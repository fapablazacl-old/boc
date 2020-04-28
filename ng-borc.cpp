
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>

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
        "main.cpp"
        "WordCounter.cpp"
        "WordCounter.hpp"
        "WordList.cpp"
        "WordList.hpp"
    });

    return package;
}

class BuildSystem {
public:
    explicit BuildSystem(Package *package_) {
        package = package_;
    }

    void build(const Compiler &compiler, const Linker linker) {
        for (Component *component : package->getComponents()) {
            this->build(compiler, linker, component);
        }
    }

private:
    void build(const Compiler &compiler, const Linker linker, const Component *component) {
        std::cout << "Compiling ..." << std::endl;

        std::vector<std::string> objects;

        for (const std::string &source : component->getSources()) {
            const std::string sourcePath = component->getPackage()->getPath() + component->getPath() + source;

            std::cout << source << " ... " << std::endl;
            const CompileOutput output = compiler.compile(sourcePath);
            output.command.execute();
            objects.push_back(output.objectFile);
        }

        std::cout << "Linking executable '" << component->getName() << "' ... " << std::endl;
        const LinkerOutput output = linker.link(component->getName(), component->getPackage()->getPath() + component->getPath() + component->getName(), objects);
        output.command.execute();
        std::cout << "Component path: '" << output.executable << "' ... " << std::endl;
        
        std::cout << "Done" << std::endl;
    }

private:
    Package *package = nullptr;
};


int main(int argc, char **argv) {
    Compiler compiler;
    Linker linker;
    // Package *package = createHelloWorldPackage();
    Package *package = createWordCounterPackage();
    
    BuildSystem buildSystem {package};

    buildSystem.build(compiler, linker);

    return 0;
}
