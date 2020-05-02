
#include "Component.hpp"

Component::Component(const Package *parentPackage, const std::string &name, const std::string &path, const std::vector<std::string> &sources) {
    this->parentPackage = parentPackage;
    this->name = name;
    this->sources = sources;
}
