project(a13bootnrecovery)
add_executable(a13bootnrecovery main.cpp)
find_library(CUPSLIB cups)
target_link_libraries(a13bootnrecovery ${CUPSLIB})

