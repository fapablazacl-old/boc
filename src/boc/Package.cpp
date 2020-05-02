
#include "Package.hpp"

#include "Component.hpp"

Package::Package(const std::string &name, const std::string &path) {
    this->name = name;
    this->path = path;
}


Component* Package::addComponent(const std::string &name, const std::string &path, const std::vector<std::string> &sources) {
    Component *component = new Component(this, name, path, sources);

    components.push_back(component);

    return component;
}
