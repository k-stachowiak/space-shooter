#include "cmp_explode_when_die.h"

cmp_explode_when_die::cmp_explode_when_die(uint32_t parent_id, uint32_t num_debris)
	: component(parent_id) 
	, _num_debris(num_debris)
	, _dead(false) {
}

void cmp_explode_when_die::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {
	if(_dead)
		gm.game_msgs.push_back(game_msg::create_spawn_expl(
			_num_debris,
			_last_x,
			_last_y,
			(_last_dx / gm.last_dt) * 0.333,
			(_last_dy / gm.last_dt) * 0.333));
}

void cmp_explode_when_die::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;
		
	case ent_msg_t::MOVE:
		_last_dx = msg.move.dx;
		_last_dy = msg.move.dy;
		break;

	case ent_msg_t::HEALTH_ETC:
		if(msg.health_etc.health <= 0.0)
			_dead = true;
		break;

	default:
		break;
	}
}
