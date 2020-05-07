
#include <iostream>

#include <bok/core/Package.hpp>
#include <bok/core/BuildCache.hpp>
#include <bok/core/BuildSystem.hpp>
#include <bok/core/Compiler.hpp>
#include <bok/core/CompilerGCC.hpp>
#include <bok/core/Linker.hpp>

using namespace bok;

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
    CompilerGCC compiler;
    Linker linker;
    BuildCache buildCache{"buildCache.txt"};

    // Package *package = createHelloWorldPackage();
    Package *package = createWordCounterPackage();
    
    BuildCommmandListener listener;
    BuildSystem buildSystem {package, &buildCache, &listener};

    buildSystem.build(compiler, linker);

    return 0;
}
