#include "cmp_move_player.h"

cmp_move_player::cmp_move_player(uint32_t parent_id)
	: component(parent_id) {
}

void cmp_move_player::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	double dx = cfg.get_player_vx() * gm.steering_vec[0] * gm.last_dt;
	double dy = cfg.get_player_vy() * gm.steering_vec[1] * gm.last_dt;

	ent_msg msg;
	msg.type = ent_msg_t::MOVE;
	msg.move.dx = dx;
	msg.move.dy = dy;
	ent_msgs.push_back(msg);
}

void cmp_move_player::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;
	default:
		break;
	}
}
