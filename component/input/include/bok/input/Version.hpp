
#pragma once 

namespace bok {
	struct Version {
		int major;
		int minor;
		int revision;

        bool operator == (const Version &rhs) const;
	};
}
