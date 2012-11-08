#include "cmp_smoke_when_hurt.h"
	
void cmp_smoke_when_hurt::reset_counter(double rest) {
	uniform_real_distribution<double> dist(0.25, 0.5);
	_counter = dist(_engine) - rest;
}

cmp_smoke_when_hurt::cmp_smoke_when_hurt(uint32_t parent_id)
: component(parent_id) {
	reset_counter(0.0);
}

void cmp_smoke_when_hurt::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	double health_ratio = _last_health / _max_health;
	if(health_ratio > 0.5)
		return;

	_counter -= gm.last_dt;
	if(_counter <= 0.0) {
		reset_counter(-_counter);
		uniform_real_distribution<double> xy_dev_dist(-30.0, 30.0);
		gm.game_msgs.push_back(game_msg::create_spawn_smoke(
			_last_x + xy_dev_dist(_engine),
			_last_y + xy_dev_dist(_engine)));

		if(health_ratio >= 0.25)
			return;

		gm.game_msgs.push_back(game_msg::create_spawn_expl(
			0,
			_last_x + xy_dev_dist(_engine),
			_last_y + xy_dev_dist(_engine),
			0.0, 0.0));
	}
}

void cmp_smoke_when_hurt::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;

	case ent_msg_t::HEALTH_ETC:
		_last_health = msg.health_etc.health;
		_max_health = msg.health_etc.max_health;
		break;

	default:
		break;
	}
}

