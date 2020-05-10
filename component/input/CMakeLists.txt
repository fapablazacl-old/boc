
set (target core)

set (sources 
    "include/bok/input/BuildCache.hpp"
    "include/bok/core/BuildSystem.hpp"
    "include/bok/core/Command.hpp"
    "include/bok/core/Compiler.hpp"
    "include/bok/core/CompilerGCC.hpp"
    "include/bok/core/Component.hpp"
    "include/bok/core/Linker.hpp"
    "include/bok/core/Package.hpp"
    
    "src/BuildCache.cpp"
    "src/BuildSystem.cpp"
    "src/Command.cpp"
    "src/Compiler.cpp"
    "src/CompilerGCC.cpp"
    "src/Component.cpp"
    "src/Linker.cpp"
    "src/Package.cpp"
)

add_library(${target} ${sources})
