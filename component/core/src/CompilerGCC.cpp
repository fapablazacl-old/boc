
#include <bok/core/CompilerGCC.hpp>


namespace bok {
    CompileOutput CompilerGCC::compile(const std::string &source) const {
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


    bool CompilerGCC::isCompilable(const std::string &source) const {
        if (auto pos = source.rfind("."); pos != std::string::npos) {
            const std::string ext = source.substr(pos, source.size());

            return ext == ".cpp";
        }

        return false;
    }
}
