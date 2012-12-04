cmake_minimum_required (VERSION 2.6)
project (shooter)

set (CMAKE_CXX_COMPILER "g++" )
set (CMAKE_CXX_FLAGS "-Wall --std=gnu++0x -g -pg")

add_executable(shooter
./src/states/test_state.cpp
./src/geometry/bezier.cpp
./src/cmp/components.cpp
./src/cmp/systems.cpp
./src/cmp/comm.cpp
./src/misc/rand.cpp
./src/allegro.cpp
./src/resources/resman.cpp
./src/main.cpp)

target_link_libraries(shooter allegro_monolith)
