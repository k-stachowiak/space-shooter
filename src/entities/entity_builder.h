#ifndef ENTITY_BUILDER_H
#define ENTITY_BUILDER_H

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

#include <random>
using std::default_random_engine;
using std::uniform_int_distribution;
using std::uniform_real_distribution;

#include "../resources/resman.h"
#include "../collision/shape.h"
#include "../collision/collision_space.h"
#include "../components/component.h"
#include "../components/appearance/frame_def.h"
#include "../geometry/types.h"
#include "entity.h"

class entity_builder {

	default_random_engine _engine;

	// Dependencies.
	const resman& _res;

	// Configuration.
	uint32_t _parent_id;

	// State.
	vector<shared_ptr<component>> _components;

public:
	entity_builder(const resman& res);

	// Builder pattern interface.
	entity_builder& begin(uint32_t parent_id);
	entity build();

	// Appearance components.
	entity_builder& add_appr_bitmap(res_id bitmap_id);
	entity_builder& add_appr_pixel(double r, double g, double b, double a);
	entity_builder& add_appr_simple_anim(res_id bitmap_id, uint32_t frame_width, uint32_t num_frames, vector<frame_def> frame_defs, int rep_count);

	// Orientation and movement components.
	entity_builder& add_move_const_speed(double vx, double vy);
	entity_builder& add_move_path(vector<point> points);
	entity_builder& add_move_player();
	entity_builder& add_rotate_const_speed(double omega);
	entity_builder& add_orientation(double x, double y, double theta);

	// Miscallaneous limitation components.
	entity_builder& add_bounded_life(double x_min, double x_max, double y_min, double y_max);
	entity_builder& add_bounded_movement(double x_min, double x_max, double y_min, double y_max);
	entity_builder& add_time_to_live(double ttl);

	// Emitter components.
	entity_builder& add_bullet_emitter(double dt_min, double dt_max, double vy, res_id bitmap_id, bool is_enemy);

	// Death related components.
	entity_builder& add_die_end_anim();
	entity_builder& add_explode_when_die(uint32_t num_debris);
	entity_builder& add_score_when_die(double score);
	entity_builder& add_drop_when_die(drop_t drop_type);
	
	// Health related components.
	entity_builder& add_health_etc(double health, double armor);
	entity_builder& add_painmap(map<collision_class, double> pain_map);
	entity_builder& add_smoke_when_hurt();
	entity_builder& add_player_health();
	entity_builder& add_health_picker(double health_boost, double armor_boost);

	// Uncathegorized components.
	entity_builder& add_collidable(collision_class cc, shared_ptr<shape> shp, collision_space& cs);
	entity_builder& add_shoot_player(double period, double max_x_dev);
};

#endif
