#include "cmp_bullet_emitter.h"

void cmp_bullet_emitter::reset_counter(double rest) {
	uniform_real_distribution<double> dist(_dt_min, _dt_max);
	_counter = dist(_engine) - rest;
}

cmp_bullet_emitter::cmp_bullet_emitter(
	uint32_t parent_id,
	double dt_min, double dt_max,
	double vy,
	res_id bitmap_id, bool is_enemy)
: component(parent_id)
, _dt_min(dt_min)
, _dt_max(dt_max)
, _vy(vy)
, _bitmap_id(bitmap_id)
, _is_enemy(is_enemy) {

	reset_counter(0.0);
}

void cmp_bullet_emitter::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	_counter -= gm.last_dt;
	if(_counter <= 0.0) {
		reset_counter(-_counter);
		gm.game_msgs.push_back(game_msg::create_spawn_bullets(
			_last_x, _last_y,
			0.0, _vy,
			_bitmap_id,
			_is_enemy));
	}
}

void cmp_bullet_emitter::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;

	default:
		break;
	}
}
