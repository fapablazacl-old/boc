
set (target boc)

set (sources 
    "boc/BuildCache.hpp"
    "boc/BuildCache.cpp"
    "boc/BuildSystem.hpp"
    "boc/BuildSystem.cpp"
    "boc/Command.hpp"
    "boc/Command.cpp"
    "boc/Compiler.hpp"
    "boc/Compiler.cpp"
    "boc/Component.hpp"
    "boc/Component.cpp"
    "boc/Linker.hpp"
    "boc/Linker.cpp"
    "boc/Package.hpp"
    "boc/Package.cpp"
    "boc/ng-borc.cpp"
)

add_executable(${target} ${sources})
