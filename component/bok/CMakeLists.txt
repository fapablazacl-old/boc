
set (target bok)

set (sources 
    "src/ng-borc.cpp"
)

add_executable(${target} ${sources})

target_link_libraries(${target} core)
