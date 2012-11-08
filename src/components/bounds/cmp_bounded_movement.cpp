#include "cmp_bounded_movement.h"

cmp_bounded_movement::cmp_bounded_movement(
	uint32_t parent_id,
	double min_x,
	double max_x,
	double min_y,
	double max_y)
: component(parent_id)
, _x_min(min_x)
, _x_max(max_x)
, _y_min(min_y)
, _y_max(max_y) {}

void cmp_bounded_movement::receive_message(
		const ent_msg& msg,
		vector<ent_msg>& ent_msgs) {

	ent_msg new_msg;
	double x, y;

	switch(msg.type) {
	case ent_msg_t::ORIENTATION:

		x = msg.orientation.x;
		y = msg.orientation.y;
		
		new_msg.type = ent_msg_t::CANCEL_MOVE;
		new_msg.cancel_move.cancel_dx = false;
		new_msg.cancel_move.cancel_dy = false;

		if(x < _x_min || x > _x_max)
			new_msg.cancel_move.cancel_dx = true;
		
		if(y < _y_min || y > _y_max)
			new_msg.cancel_move.cancel_dy = true;

		if(new_msg.cancel_move.cancel_dx || new_msg.cancel_move.cancel_dy)
			ent_msgs.push_back(new_msg);

		break;

	default:
		break;
	}
}

