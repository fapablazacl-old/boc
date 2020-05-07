
#pragma once 

#include <string>
#include <vector>


namespace bok {
    class Command {
    public:
        explicit Command(const std::string &path_, const std::string &name_);

        explicit Command(const std::string &name_);

        Command& addArg(const std::string &arg);

        void execute() const;
        
    private:
        std::string path;
        std::string name;
        std::vector<std::string> args;
    };
}
