
#pragma once 

#include <string>

#include "Dependency.hpp"

namespace bok {
	struct ComponentLanguage {
		std::string name;
		std::string dialect;

		explicit ComponentLanguage(const std::string &value);

		~ComponentLanguage();
	};
	
	struct ComponentType {
		std::string name;
		std::string kind;

		explicit ComponentType(const std::string &value);

		~ComponentType();
	};
	
	struct Component {
		std::string name;
		std::string description;
		ComponentType type;
		std::vector<ComponentLanguage> languages;
		std::vector<std::string> sources;
		std::vector<Dependency> dependencies;

		~Component();
	};
}
