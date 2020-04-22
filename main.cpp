
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
            Command{"clang"}
                .addArg("-std=c++17")
                .addArg("-c")
                .addArg(source)
                .addArg("-O0")
                .addArg("-g")
                .addArg("-o" + object)
        };
    }

private:
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
    LinkerOutput link(const std::string &name, const std::vector<std::string> &objects) const {
        assert(objects.size());

        Command command("ld");

        for (const std::string &object : objects) {
            command.addArg(object);
        }

        return LinkerOutput {
            objects,
            name,
            command
                .addArg("-lc++")
                .addArg("-lm")
                .addArg("-o " + name)
                .addArg("-macosx_version_min " "10.14")
        };
    }
};


class Component {
public:
    std::vector<std::string> getSources() const {
        return {
            "main.cpp"
        };
    }

    std::string getName() const {
        return "borc";
    }
};


class Package {
public:
    std::vector<Component> getComponents() const {
        return {
            Component{}
        };
    }
};


class BuildSystem {
public:
    explicit BuildSystem(Package *package_) {
        package = package_;
    }


    void build(const Compiler &compiler, const Linker linker) {
        for (Component component : package->getComponents()) {
            this->build(compiler, linker, component);
        }
    }

private:
    void build(const Compiler &compiler, const Linker linker, const Component &component) {
        std::cout << "Compiling ..." << std::endl;

        std::vector<std::string> objects;

        for (const std::string &source : component.getSources()) {
            std::cout << source << " ... " << std::endl;
            const CompileOutput output = compiler.compile(source);
            output.command.execute();
            objects.push_back(output.objectFile);
        }

        std::cout << "Linking executable '" << component.getName() << "' ... " << std::endl;
        const LinkerOutput output = linker.link(component.getName(), objects);
        output.command.execute();
        
        std::cout << "Done" << std::endl;
    }

private:
    Package *package = nullptr;
};


int main(int argc, char **argv) {
    Compiler compiler;
    Linker linker;
    Package package;

    BuildSystem buildSystem{&package};

    buildSystem.build(compiler, linker);

    return 0;
}
