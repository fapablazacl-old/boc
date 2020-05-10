
#pragma once 

#include <string>
#include <vector>
#include <optional>

#include "Version.hpp"

namespace bok {
	struct DependencyVersion {
		enum class Restriction {
			GreaterOrEqual,
			Equal,
			LesserOrEqual
		};
		
		Version version;
		Restriction restriction;
	};
	
	
	struct Dependency {
		Dependency() = default;
		Dependency(const Dependency &other) = default;
		
		explicit Dependency(const std::string &value);
		
		std::string packageName;
		std::vector<std::string> componentNames;
		
		std::optional<DependencyVersion> dependencyVersion;
	};
}
