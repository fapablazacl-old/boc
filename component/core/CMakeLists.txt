
set (target core)

set (sources 
    "include/boc/core/BuildCache.hpp"
    "include/boc/core/BuildSystem.hpp"
    "include/boc/core/Command.hpp"
    "include/boc/core/Compiler.hpp"
    "include/boc/core/Component.hpp"
    "include/boc/core/Linker.hpp"
    "include/boc/core/Package.hpp"
    
    "src/BuildCache.cpp"
    "src/BuildSystem.cpp"
    "src/Command.cpp"
    "src/Compiler.cpp"
    "src/Component.cpp"
    "src/Linker.cpp"
    "src/Package.cpp"
)

add_library(${target} ${sources})

