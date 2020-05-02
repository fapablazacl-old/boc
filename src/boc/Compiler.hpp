
#pragma once 

#include "Command.hpp"

struct CompileOutput {
    std::string sourceFile;
    std::string objectFile;
    Command command;
};


class Compiler {
public:
    CompileOutput compile(const std::string &source) const;

    bool isCompilable(const std::string &source) const;

private:
    Command createCompilerCommand() const {
        // return Command{"clang"};
        return Command{"gcc"};
    }


    std::string objectName(const std::string &source) const {
        return source + ".obj";
    }
};
