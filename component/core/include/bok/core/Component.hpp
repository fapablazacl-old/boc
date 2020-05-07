
#pragma once 

#include <string>
#include <vector>

namespace bok {
    class Package;
    class Component {
    public:
        explicit Component(const Package *parentPackage, const std::string &name, const std::string &path, const std::vector<std::string> &sources);


        std::vector<std::string> getSources() const {
            return sources;
        }


        std::string getName() const {
            return name;
        }


        const Package* getPackage() const {
            return parentPackage;
        }


        std::string getPath() const {
            return path;
        }

    private:
        const Package *parentPackage = nullptr;
        std::string name;
        std::string path;
        std::vector<std::string> sources;
    };
}
