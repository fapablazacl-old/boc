
#pragma once 

#include "Command.hpp"

namespace bok {
    struct CompileOutput {
        std::string sourceFile;
        std::string objectFile;
        Command command;
    };


    class Compiler {
    public:
        virtual ~Compiler();

        virtual CompileOutput compile(const std::string &source) const = 0;

        virtual bool isCompilable(const std::string &source) const = 0;
    };
}
