
#ifndef __BOK_COMPILERGCC_HPP__
#define __BOK_COMPILERGCC_HPP__

#include "Compiler.hpp"

namespace bok {
    class CompilerGCC : public Compiler {
    public:
        virtual ~CompilerGCC() {}

        CompileOutput compile(const std::string &source) const override;

        bool isCompilable(const std::string &source) const override;

    private:
        Command createCompilerCommand() const {
            // return Command{"clang"};
            return Command{"gcc"};
        }


        std::string objectName(const std::string &source) const {
            return source + ".obj";
        }
    };
}

#endif
