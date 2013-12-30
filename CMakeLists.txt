cmake_minimum_required(VERSION 2.8)

project(shoot)

link_directories(/usr/local/lib)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -O0")

add_executable(shoot
        ./src/script/dom.cpp
        ./src/script/expect.cpp
        ./src/states/new_engine_state.cpp
        ./src/states/coll_test_state.cpp
        ./src/states/hs_enter_state.cpp
        ./src/states/menu_state.cpp
        ./src/states/hs_display_state.cpp
        ./src/states/game_state.cpp
        ./src/allegro.cpp
        ./src/gameplay/wave.cpp
        ./src/misc/logger.cpp
        ./src/misc/high_score.cpp
        ./src/misc/rand.cpp
        ./src/misc/config.cpp
        ./src/main.cpp
        ./src/efwk/game_ent_fac.cpp
        ./src/efwk/sys/pain.cpp
        ./src/efwk/sys/wellness.cpp
        ./src/efwk/sys/input.cpp
        ./src/efwk/sys/pickup.cpp
        ./src/efwk/sys/fx.cpp
        ./src/efwk/sys/collision.cpp
        ./src/efwk/sys/score.cpp
        ./src/efwk/sys/movement.cpp
        ./src/efwk/sys/drawing.cpp
        ./src/efwk/sys/sound.cpp
        ./src/efwk/sys/hud.cpp
        ./src/efwk/sys/arms.cpp
        ./src/efwk/cmp/reaction.cpp
        ./src/efwk/cmp/bounds.cpp
        ./src/efwk/cmp/dynamics.cpp
        ./src/efwk/cmp/wellness.cpp
        ./src/efwk/cmp/appr.cpp
        ./src/efwk/cmp/coll_queue.cpp
        ./src/efwk/cmp/timer.cpp
        ./src/efwk/cmp/fx.cpp
        ./src/efwk/cmp/orientation.cpp
        ./src/efwk/cmp/col_profile.cpp
        ./src/efwk/cmp/upgrades.cpp
        ./src/efwk/cmp/shape.cpp
        ./src/efwk/cmp/pickup_profile.cpp
        ./src/efwk/cmp/weapon_beh.cpp
        ./src/efwk/comm.cpp
        ./src/resources/resman.cpp
        ./src/geometry/bezier.cpp)

target_link_libraries(shoot allegro_monolith)
