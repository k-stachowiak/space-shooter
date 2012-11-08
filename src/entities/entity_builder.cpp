#include "entity_builder.h"

#include "../components/appearance/cmp_appr_bitmap.h"
#include "../components/appearance/cmp_appr_pixel.h"
#include "../components/appearance/cmp_appr_simple_anim.h"
#include "../components/bounds/cmp_bounded_life.h"
#include "../components/bounds/cmp_bounded_movement.h"
#include "../components/emitters/cmp_bullet_emitter.h"
#include "../components/position/cmp_move_const_speed.h"
#include "../components/position/cmp_move_path.h"
#include "../components/position/cmp_move_player.h"
#include "../components/position/cmp_rotate_const_speed.h"
#include "../components/position/cmp_orientation.h"
#include "../components/wellness/cmp_health_etc.h"
#include "../components/wellness/cmp_painmap.h"
#include "../components/wellness/cmp_player_health.h"
#include "../components/wellness/cmp_smoke_when_hurt.h"
#include "../components/wellness/cmp_health_picker.h"
#include "../components/cmp_collidable.h"
#include "../components/cmp_die_end_anim.h"
#include "../components/cmp_explode_when_die.h"
#include "../components/cmp_drop_when_die.h"
#include "../components/cmp_score_when_die.h"
#include "../components/cmp_shoot_player.h"
#include "../components/cmp_time_to_live.h"

entity_builder::entity_builder(const resman& res)
: _res(res) {}

entity_builder& entity_builder::begin(uint32_t parent_id) {
	_parent_id = parent_id;
	_components.clear();
	return *this;
}

entity entity_builder::build() {
	entity result(_parent_id, _components);
	return result;
}

entity_builder& entity_builder::add_appr_bitmap(res_id bitmap_id) {
	_components.push_back(shared_ptr<component>(new cmp_appr_bitmap(
		_parent_id,
		_res.get_bitmap(bitmap_id))));
	return *this;
}

entity_builder& entity_builder::add_appr_pixel(double r, double g, double b, double a) {
	_components.push_back(shared_ptr<component>(new cmp_appr_pixel(
		_parent_id,
		r, g, b, a)));
	return *this;
}

entity_builder& entity_builder::add_appr_simple_anim(res_id bitmap_id, uint32_t frame_width, uint32_t num_frames, vector<frame_def> frame_defs, int rep_count) {
	_components.push_back(shared_ptr<component>(new cmp_appr_simple_anim(
		_parent_id,
		_res.get_bitmap(bitmap_id),
		frame_width,
		num_frames,
		frame_defs,
		rep_count)));
	return *this;
}

entity_builder& entity_builder::add_move_const_speed(double vx, double vy) {
	_components.push_back(shared_ptr<component>(new cmp_move_const_speed(
		_parent_id,
		vx, vy)));
	return *this;
}

entity_builder& entity_builder::add_move_path(vector<point> points) {
	_components.push_back(shared_ptr<component>(new cmp_move_path(
		_parent_id,
		points)));
	return *this;
}

entity_builder& entity_builder::add_move_player() {
	_components.push_back(shared_ptr<component>(new cmp_move_player(
		_parent_id)));
	return *this;
}

entity_builder& entity_builder::add_rotate_const_speed(double omega) {
	_components.push_back(shared_ptr<component>(new cmp_rotate_const_speed(
		_parent_id, omega)));
	return *this;
}

entity_builder& entity_builder::add_orientation(double x, double y, double theta) {
	_components.push_back(shared_ptr<component>(new cmp_orientation(
		_parent_id,
		x, y, theta)));
	return *this;
}

entity_builder& entity_builder::add_bounded_life(double x_min, double x_max, double y_min, double y_max) {
	_components.push_back(shared_ptr<component>(new cmp_bounded_life(
		_parent_id,
		x_min, x_max,
		y_min, y_max)));
	return *this;
}

entity_builder& entity_builder::add_bounded_movement(double x_min, double x_max, double y_min, double y_max) {
	_components.push_back(shared_ptr<component>(new cmp_bounded_movement(
		_parent_id,
		x_min, x_max,
		y_min, y_max)));
	return *this;
}

entity_builder& entity_builder::add_time_to_live(double ttl) {
	_components.push_back(shared_ptr<component>(new cmp_time_to_live(
		_parent_id,
		ttl)));
	return *this;
}

entity_builder& entity_builder::add_bullet_emitter(double dt_min, double dt_max, double vy, res_id bitmap_id, bool is_enemy) {
	_components.push_back(shared_ptr<component>(new cmp_bullet_emitter(
		_parent_id,
		dt_min, dt_max, vy,
		bitmap_id,
		is_enemy)));
	return *this;
}

entity_builder& entity_builder::add_die_end_anim() {
	_components.push_back(shared_ptr<component>(new cmp_die_end_anim(
		_parent_id)));
	return *this;
}

entity_builder& entity_builder::add_explode_when_die(uint32_t num_debris) {
	_components.push_back(shared_ptr<component>(new cmp_explode_when_die(
		_parent_id, num_debris)));
	return *this;
}

entity_builder& entity_builder::add_score_when_die(double score) {
	_components.push_back(shared_ptr<component>(new cmp_score_when_die(
		_parent_id,
		score)));
	return *this;
}

entity_builder& entity_builder::add_drop_when_die(drop_t drop_type) {
	_components.push_back(shared_ptr<component>(new cmp_drop_when_die(
		_parent_id,
		drop_type)));
	return *this;
}

entity_builder& entity_builder::add_health_etc(double health, double armor) {
	_components.push_back(shared_ptr<component>(new cmp_health_etc(
		_parent_id,
		health, armor)));
	return *this;
}

entity_builder& entity_builder::add_painmap(map<collision_class, double> pain_map) {
	_components.push_back(shared_ptr<component>(new cmp_painmap(
		_parent_id, pain_map)));
	return *this;
}

entity_builder& entity_builder::add_smoke_when_hurt() {
	_components.push_back(shared_ptr<component>(new cmp_smoke_when_hurt(
		_parent_id)));
	return *this;
}

entity_builder& entity_builder::add_player_health() {
	_components.push_back(shared_ptr<component>(new cmp_player_health(
		_parent_id)));
	return *this;
}

entity_builder& entity_builder::add_health_picker(
		double health_boost,
		double armor_boost) {
	_components.push_back(shared_ptr<component>(new cmp_health_picker(
		_parent_id,
		health_boost, armor_boost)));
	return *this;
}

entity_builder& entity_builder::add_collidable(collision_class cc, shared_ptr<shape> shp, collision_space& cs) {
	_components.push_back(shared_ptr<component>(new cmp_collidable(
		_parent_id,
		cc, shp, cs)));
	return *this;
}

entity_builder& entity_builder::add_shoot_player(double period, double max_x_dev) {
	_components.push_back(shared_ptr<component>(new cmp_shoot_player(
		_parent_id, period, max_x_dev)));
	return *this;
}

