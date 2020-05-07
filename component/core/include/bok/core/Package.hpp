
#pragma once 

#include <string>
#include <vector>

namespace bok {
    class Component;
    class Package {
    public:
        explicit Package(const std::string &name, const std::string &path);

        Component* addComponent(const std::string &name, const std::string &path, const std::vector<std::string> &sources);
        
        std::vector<Component*> getComponents() const {
            return components;
        }

        std::string getPath() const {
            return path;
        }

    private:
        std::string name;
        std::string path;
        std::vector<Component*> components;
    };
}
