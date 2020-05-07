
#include <bok/core/Linker.hpp>

#include <cassert>


namespace bok {
    LinkerOutput Linker::link(const std::string &name, const std::string &outputFilePath, const std::vector<std::string> &objects) const {
        assert(objects.size());

        Command command("gcc");

        for (const std::string &object : objects) {
            command.addArg(object);
        }

        bool is_macOS = false;

        if (is_macOS) {
            command.addArg("-macosx_version_min " "10.14");
            command.addArg("-lc++");
        } else {
            command.addArg("-lstdc++");
        }

        return LinkerOutput {
            objects,
            outputFilePath,
            command
                .addArg("-lm")
                .addArg("-o " + outputFilePath)
        };
    }
}
