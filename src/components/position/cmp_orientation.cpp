#include "cmp_orientation.h"

ent_msg cmp_orientation::orientation_msg() {
	ent_msg msg;
	msg.type = ent_msg_t::ORIENTATION;
	msg.orientation.x = _x;
	msg.orientation.y = _y;
	msg.orientation.theta = _theta;
	return msg;
}

cmp_orientation::cmp_orientation(
	uint32_t parent_id,
	double x, 
	double y, 
	double theta)
	: component(parent_id)
	, _prev_x(x)
	, _prev_y(y)
	, _x(x)
	, _y(y)
	, _theta(theta){
}

void cmp_orientation::initialize(vector<ent_msg>& ent_msgs) {
	ent_msgs.push_back(orientation_msg());
}

void cmp_orientation::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	_prev_x = _x;
	_prev_y = _y;
}

void cmp_orientation::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::MOVE:
		_x += msg.move.dx;
		_y += msg.move.dy;
		ent_msgs.push_back(orientation_msg());
		break;

	case ent_msg_t::CANCEL_MOVE:
		if(msg.cancel_move.cancel_dx) {
			_x = _prev_x;
		}
		if(msg.cancel_move.cancel_dy) {
			_y = _prev_y;
		}
		ent_msgs.push_back(orientation_msg());
		break;

	case ent_msg_t::ROTATE:
		_theta += msg.rotate.dtheta;
		ent_msgs.push_back(orientation_msg());
		break;

	default:
		break;
	}
}
