
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <map>
#include <fstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>

class Command {
public:
    explicit Command(const std::string &path_, const std::string &name_) 
        : path(path_), name(name_) {}


    explicit Command(const std::string &name_) 
        : name(name_) {}


    Command& addArg(const std::string &arg) {
        args.push_back(arg);

        return *this;
    }


    void execute() const {
        std::string cmdline = path + name;

        for (const std::string &arg : args) {
            cmdline += " " + arg;
        }

        if (int exitCode = std::system(cmdline.c_str()); exitCode != 0) {
            throw std::runtime_error("The following command failed: " + cmdline);
        }
    }
    
private:
    std::string path;
    std::string name;
    std::vector<std::string> args;
};


struct CompileOutput {
    std::string sourceFile;
    std::string objectFile;
    Command command;
};


class Compiler {
public:
    CompileOutput compile(const std::string &source) const {
        // std::cout << "clang -c " << source << " " << "-O0" << " " << "-g" << " " << "-o" << objectName(source) << std::endl;
        const std::string object = objectName(source);

        return CompileOutput {
            source, 
            object, 
            createCompilerCommand()
                .addArg("-std=c++17")
                .addArg("-c")
                .addArg(source)
                .addArg("-O0")
                .addArg("-g")
                .addArg("-o" + object)
        };
    }


    bool isCompilable(const std::string &source) const {
        if (auto pos = source.rfind("."); pos != std::string::npos) {
            const std::string ext = source.substr(pos, source.size());

            return ext == ".cpp";
        }

        return false;
    }

private:
    Command createCompilerCommand() const {
        // return Command{"clang"};
        return Command{"gcc"};
    }


    std::string objectName(const std::string &source) const {
        return source + ".obj";
    }
};


struct LinkerOutput {
    std::vector<std::string> objectFiles;
    std::string executable;
    Command command;
};


class Linker {
public:
    LinkerOutput link(const std::string &name, const std::string &outputFilePath, const std::vector<std::string> &objects) const {
        assert(objects.size());

        Command command("gcc");

        for (const std::string &object : objects) {
            command.addArg(object);
        }

        bool is_macOS = false;

        if (is_macOS) {
            command.addArg("-macosx_version_min " "10.14");
            command.addArg("-lc++");
        } else {
            command.addArg("-lstdc++");
        }

        return LinkerOutput {
            objects,
            outputFilePath,
            command
                .addArg("-lm")
                .addArg("-o " + outputFilePath)
        };
    }
};


class Package;
class Component {
public:
    explicit Component(const Package *parentPackage, const std::string &name, const std::string &path, const std::vector<std::string> &sources) {
        this->parentPackage = parentPackage;
        this->name = name;
        this->sources = sources;
    }


    std::vector<std::string> getSources() const {
        return sources;
    }


    std::string getName() const {
        return name;
    }


    const Package* getPackage() const {
        return parentPackage;
    }


    std::string getPath() const {
        return path;
    }
private:
    const Package *parentPackage = nullptr;
    std::string name;
    std::string path;
    std::vector<std::string> sources;
};


class Component;
class Package {
public:
    explicit Package(const std::string &name, const std::string &path) {
        this->name = name;
        this->path = path;
    }

    std::vector<Component*> getComponents() const {
        return components;
    }

    Component* addComponent(const std::string &name, const std::string &path, const std::vector<std::string> &sources) {
        Component *component = new Component(this, name, path, sources);

        components.push_back(component);

        return component;
    }

    std::string getPath() const {
        return path;
    }

private:
    std::string name;
    std::string path;
    std::vector<Component*> components;
};


Package* createBorcPackage() {
    auto package = new Package("ng-borc", "./");

    package->addComponent("borc", "./", {
        "main.cpp"
    });

    return package;
}


Package* createHelloWorldPackage() {
    auto package = new Package("01-hello-world", "./test-data/cpp-core/01-hello-world/");

    package->addComponent("01-hello-world", "./", {
        "main.cpp"
    });

    return package;
}


Package* createWordCounterPackage() {
    auto package = new Package("02-word-counter", "./test-data/cpp-core/02-word-counter/");

    package->addComponent("02-word-counter", "./", {
        "main.cpp",
        "WordCounter.cpp",
        "WordCounter.hpp",
        "WordList.cpp",
        "WordList.hpp"
    });

    return package;
}


class BuildCache {
public:
    BuildCache(const std::string &cacheFile) {
        this->cacheFile = cacheFile;

        std::fstream fs(cacheFile.c_str(), std::ios_base::in);

        if (! fs.is_open()) {
            return;
        }

        std::string line;

        while (!fs.eof()) {
            std::getline(fs, line);

            size_t pos = line.find(':');

            if (pos == std::string::npos) {
                continue;
            }

            const std::string key = line.substr(0, pos);
            const time_t value = static_cast<time_t>(std::atol(line.substr(pos, line.size()).c_str()));

            sourceCache.insert({key, value});
        }
    }


    ~BuildCache() {
        std::fstream fs(cacheFile.c_str(), std::ios_base::out);

        if (! fs.is_open()) {
            return;
        }

        for (const auto &pair : sourceCache) {
            fs << "\"" << pair.first << "\":" << pair.second << std::endl;
        }
    }


    void sourceBuilt(const char *sourceFile) {
        const std::string key = sourceFile;
        const time_t value = this->getModifiedTime(sourceFile, false).value();

        sourceCache.insert({key, value});
    }


    bool sourceNeedsRebuild(const char *sourceFile) const {
        const time_t cachedTimestamp = this->getModifiedTime(sourceFile, true).value();
        const time_t currentTimestamp = this->getModifiedTime(sourceFile, false).value();

        return cachedTimestamp != currentTimestamp;
    }


private:
    std::optional<time_t> getModifiedTime(const char *fileName, bool cached) const {
        if (cached) {
            if (auto it = sourceCache.find(fileName); it != sourceCache.end()) {
                return it->second;
            }

            return {};
        } else {
            struct stat result;

            if (stat(fileName, &result) == 0) {
                auto modified_time = result.st_mtim;

                return modified_time.tv_sec;
            }

            return {};
        }
    }

private:
    std::string cacheFile;
    std::map<std::string, time_t> sourceCache;
};


class BuildSystem {
public:
    class Listener {
    public:
        virtual ~Listener() {}
        
        virtual void receiveOutput(const CompileOutput &output) = 0;

        virtual void receiveOutput(const LinkerOutput &output) = 0;
    };

public:
    explicit BuildSystem(Package *package, Listener *listener = nullptr) {
        this->package = package;
        this->listener = listener;
    }


    void build(const Compiler &compiler, const Linker linker) {
        for (Component *component : package->getComponents()) {
            this->build(compiler, linker, component);
        }
    }

private:
    void build(const Compiler &compiler, const Linker linker, const Component *component) {
        std::vector<std::string> objects;

        for (const std::string &source : component->getSources()) {
            if (! compiler.isCompilable(source)) {
                continue;
            }
            
            const std::string sourceFile = component->getPackage()->getPath() + component->getPath() + source;

            if (! this->isSourceChanged(sourceFile)) {
                continue;
            }

            const CompileOutput output = compiler.compile(sourceFile);

            if (listener) listener->receiveOutput(output);
            
            objects.push_back(output.objectFile);
        }

        const LinkerOutput output = linker.link(component->getName(), component->getPackage()->getPath() + component->getPath() + component->getName(), objects);
        if (listener) listener->receiveOutput(output);
    }


    bool isSourceChanged(const std::string &source) {
        return true;
    }

private:
    Package *package = nullptr;
    Listener *listener = nullptr;
};


class BuildCommmandListener : public BuildSystem::Listener {
public:
    virtual void receiveOutput(const CompileOutput &output) override {
        std::cout << "[C++] " << output.sourceFile << " ..." << std::endl;
        output.command.execute();
    }


    virtual void receiveOutput(const LinkerOutput &output) override {
        std::cout << "[C++] Linking executable ... " << std::endl;
        output.command.execute();
        std::cout << "Component path: '" << output.executable << "' ... " << std::endl;
    }
};


int main(int argc, char **argv) {
    Compiler compiler;
    Linker linker;
    // Package *package = createHelloWorldPackage();
    Package *package = createWordCounterPackage();
    
    BuildCommmandListener listener;
    BuildSystem buildSystem {package, &listener};

    buildSystem.build(compiler, linker);

    return 0;
}
