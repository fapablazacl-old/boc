
#pragma once 

#include "Command.hpp"

namespace bok {
    struct LinkerOutput {
        std::vector<std::string> objectFiles;
        std::string executable;
        Command command;
    };


    class Linker {
    public:
        LinkerOutput link(const std::string &name, const std::string &outputFilePath, const std::vector<std::string> &objects) const;
    };
}
