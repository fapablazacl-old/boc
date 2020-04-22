
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>


class AbstractCommand {
public:
    virtual ~AbstractCommand() {}
    
    virtual void execute() = 0;
};


class Command : public AbstractCommand {
public:
    Command(const std::string &path_, const std::string &name_) 
        : path(path_), name(name_) {}


    void addArg(const std::string &arg) {
        args.push_back(arg);
    }


    void execute() override {
        std::string cmdline = path + name;

        for (const std::string &arg : args) {
            cmdline += arg + " ";
        }

        if (int exitCode = std::system(cmdline.c_str()); !exitCode) {
            throw std::runtime_error("The following command failed: " + cmdline);
        }
    }
    
private:
    std::string path;
    std::string name;
    std::vector<std::string> args;
};


class Compiler {
public:
    std::string compile(const std::string &source) const {
        std::cout << "clang -c " << source << " " << "-O0" << " " << "-g" << " " << "-o" << objectName(source) << std::endl;

        return objectName(source);
    }

private:
    std::string objectName(const std::string &source) const {
        return source + ".obj";
    }
};


class Linker {
public:
    std::string link(const std::string &name, const std::vector<std::string> &objects) const {
        assert(objects.size());

        std::cout << "Linking executable '" << name << "' ... " << std::endl;

        return name;
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


class BuildSystem {
public:
    void build(const Compiler &compiler, const Linker linker, const Component &component){
        std::cout << "Compiling ..." << std::endl;

        std::vector<std::string> objects;

        for (const std::string &source : component.getSources()) {
            objects.push_back(compiler.compile(source));
        }

        const std::string executable = linker.link(component.getName(), objects);

        std::cout << "Done" << std::endl;
    }
};


int main(int argc, char **argv) {
    Compiler compiler;
    Linker linker;
    Component component;

    BuildSystem buildSystem;

    buildSystem.build(compiler, linker, component);

    return 0;
}
