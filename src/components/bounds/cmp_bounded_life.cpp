#include "cmp_bounded_life.h"

cmp_bounded_life::cmp_bounded_life(uint32_t parent_id, double x_min, double x_max, double y_min, double y_max)
	: component(parent_id)
	, _x_min(x_min)
	, _x_max(x_max)
	, _y_min(y_min)
	, _y_max(y_max) {
}

void cmp_bounded_life::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {

	if(	_last_x < _x_min || _last_x > _x_max ||
		_last_y < _y_min || _last_y > _y_max) {
			gm.game_msgs.push_back(game_msg::create_remove_entity(_parent_id));
	}
}

void cmp_bounded_life::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;

	default:
		break;
	}
}
