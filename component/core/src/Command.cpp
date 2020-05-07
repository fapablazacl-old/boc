
#include <bok/core/Command.hpp>

#include <cstdlib>
#include <stdexcept>


namespace bok {
    Command::Command(const std::string &path_, const std::string &name_) 
        : path(path_), name(name_) {}


    Command::Command(const std::string &name_) 
        : name(name_) {}


    Command& Command::addArg(const std::string &arg) {
        args.push_back(arg);

        return *this;
    }


    void Command::execute() const {
        std::string cmdline = path + name;

        for (const std::string &arg : args) {
            cmdline += " " + arg;
        }

        if (int exitCode = std::system(cmdline.c_str()); exitCode != 0) {
            throw std::runtime_error("The following command failed: " + cmdline);
        }
    }
}
