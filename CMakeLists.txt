cmake_minimum_required (VERSION 2.6)
project (shooter)

set (CMAKE_CXX_COMPILER "g++" )
set (CMAKE_CXX_FLAGS "-Wall --std=gnu++0x -g -pg")

add_executable(shooter
./src/game_state.cpp
./src/test_state.cpp
./src/allegro.cpp
./src/resources/resman.cpp
./src/cmp/components.cpp
./src/cmp/systems.cpp
./src/cmp/comm.cpp
./src/components/wellness/cmp_health_etc.cpp
./src/components/wellness/cmp_painmap.cpp
./src/components/wellness/cmp_smoke_when_hurt.cpp
./src/components/wellness/cmp_player_health.cpp
./src/components/wellness/cmp_health_picker.cpp
./src/components/bounds/cmp_bounded_life.cpp
./src/components/bounds/cmp_bounded_movement.cpp
./src/components/position/cmp_move_const_speed.cpp
./src/components/position/cmp_move_path.cpp
./src/components/position/cmp_orientation.cpp
./src/components/position/cmp_move_player.cpp
./src/components/position/cmp_rotate_const_speed.cpp
./src/components/cmp_explode_when_die.cpp
./src/components/cmp_drop_when_die.cpp
./src/components/cmp_score_when_die.cpp
./src/components/appearance/cmp_appr_bitmap.cpp
./src/components/appearance/cmp_appr_simple_anim.cpp
./src/components/appearance/cmp_appr_pixel.cpp
./src/components/cmp_time_to_live.cpp
./src/components/cmp_shoot_player.cpp
./src/components/emitters/cmp_bullet_emitter.cpp
./src/components/cmp_die_end_anim.cpp
./src/components/cmp_collidable.cpp
./src/entities/entity_factory.cpp
./src/entities/entity_builder.cpp
./src/entities/entity.cpp
./src/main.cpp
./src/geometry/bezier.cpp
./src/collision/circle.cpp
./src/collision/collision_space.cpp
./src/collision/collisions.cpp)

target_link_libraries(shooter allegro_monolith)
