
#include <bok/core/input/Version.hpp>

#include <tuple>

namespace bok {
    bool Version::operator== (const Version &rhs) const {
        return std::tuple(major, minor, revision) == std::tuple(rhs.major, rhs.minor, rhs.revision);
    }
}
