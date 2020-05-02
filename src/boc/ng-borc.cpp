
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <map>
#include <fstream>
#include <algorithm>
#include <memory>

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
private:
    enum DATA_LOCATION {
        DL_CACHE,
        DL_FILESYSTEM
    };


public:
    explicit BuildCache(const std::string &cacheFile) {
        this->cacheFile = cacheFile;
        this->loadCache();

        fsOutput.open (cacheFile.c_str(), std::ios_base::out);
        if (! fsOutput.is_open()) {
            return;
        }
    }


    ~BuildCache() {
        this->saveCache();
    }


    void sourceBuilt(const std::string &sourceFile) {
        const time_t modifiedTime = this->getModifiedTime(sourceFile.c_str(), DL_FILESYSTEM).value();

        sourceCache.insert({sourceFile, modifiedTime});

        this->appendEntryToCache(sourceFile, modifiedTime);
    }


    bool sourceNeedsRebuild(const std::string &sourceFile) const {
        std::cout << "Cheking " << sourceFile << std::endl;

        const auto cachedTimestamp = this->getModifiedTime(sourceFile.c_str(), DL_CACHE);
        const auto currentTimestamp = this->getModifiedTime(sourceFile.c_str(), DL_FILESYSTEM);

        if (!cachedTimestamp.has_value() || !currentTimestamp.has_value()) {
            std::cout << "Must build (HARD): " << true << std::endl;
            std::cout << "    cachedTimestamp: " << cachedTimestamp.has_value() << std::endl;
            std::cout << "    currentTimestamp: " << currentTimestamp.has_value() << std::endl;

            return true;
        }

        std::cout << "Must build: " << (cachedTimestamp.value() != currentTimestamp.value()) << std::endl;
        return cachedTimestamp.value() != currentTimestamp.value();
    }

private:
    void loadCache() {
        std::fstream fs(cacheFile.c_str(), std::ios_base::in);

        if (! fs.is_open()) {
            return;
        }

        std::string line;

        while (!fs.eof()) {
            std::getline(fs, line);

            std::cout << "Parsing line: " << line << std::endl;

            size_t pos = line.find(':');

            if (pos == std::string::npos) {
                std::cout << "    Separator not found: " << line << std::endl;
                continue;
            }

            const std::string key = line.substr(0, pos);
            const std::string value_str = line.substr(pos + 1, line.size());
            const time_t value = static_cast<time_t>(std::atol(line.substr(pos + 1, line.size()).c_str()));
            const long long_value = std::atol(line.substr(pos + 1, line.size()).c_str());

            std::cout << "    key: " << key << std::endl;
            std::cout << "    value_str: " << value_str << std::endl;
            std::cout << "    value: " << value << std::endl;
            std::cout << "    long_value: " << long_value << std::endl;

            sourceCache.insert({key, value});
        }

        // 
        std::cout << "Loaded cache: " << std::endl;
        for (const auto &pair : sourceCache) {
            std::cout << "    " << "\"" << pair.first << "\":" << pair.second << std::endl;
        }
    }

    
    void saveCache() {
        std::fstream fs {cacheFile.c_str(), std::ios_base::out};

        if (! fs.is_open()) {
            return;
        }

        for (const auto &pair : sourceCache) {
            fs << pair.first << ":" << pair.second << std::endl;
        }
    }
    

    void appendEntryToCache(const std::string &sourceFile, const time_t modifiedTime) {
        // std::cout << "appendEntryToCache: " << sourceFile << ":" << modifiedTime << std::endl;

        fsOutput  << sourceFile << ":" << modifiedTime << std::endl;
        fsOutput.flush();
    }


    std::optional<time_t> getModifiedTime(const char *fileName, DATA_LOCATION location) const {
        switch (location) {
            case DL_CACHE: {
                if (auto it = sourceCache.find(fileName); it != sourceCache.end()) {
                    return it->second;
                }

                break;
            }

            case DL_FILESYSTEM: {
                struct stat result;

                if (stat(fileName, &result) == 0) {
                    auto modified_time = result.st_mtim;

                    return modified_time.tv_sec;
                }

                break;
            }
        }

        return {};
    }

private:
    std::string cacheFile;
    std::map<std::string, time_t> sourceCache;
    std::fstream fsOutput;
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
    explicit BuildSystem(Package *package, BuildCache *buildCache, Listener *listener = nullptr) {
        this->package = package;
        this->buildCache = buildCache;
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
            const CompileOutput output = compiler.compile(sourceFile);

            if (buildCache->sourceNeedsRebuild(sourceFile)) {
                if (listener) {
                    listener->receiveOutput(output);
                    buildCache->sourceBuilt(sourceFile);
                }
            }

            objects.push_back(output.objectFile);
        }

        const LinkerOutput output = linker.link(component->getName(), component->getPackage()->getPath() + component->getPath() + component->getName(), objects);

        if (listener) {
            listener->receiveOutput(output);
        }
    }


private:
    Package *package = nullptr;
    BuildCache *buildCache = nullptr;
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
    BuildCache buildCache{"buildCache.txt"};

    // Package *package = createHelloWorldPackage();
    Package *package = createWordCounterPackage();
    
    BuildCommmandListener listener;
    BuildSystem buildSystem {package, &buildCache, &listener};

    buildSystem.build(compiler, linker);

    return 0;
}
