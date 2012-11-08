#include "cmp_move_const_speed.h"


cmp_move_const_speed::cmp_move_const_speed(uint32_t parent_id, double vx, double vy)
	: component(parent_id)
	, _vx(vx)
	, _vy(vy) {
}

void cmp_move_const_speed::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {

	double dx = _vx * gm.last_dt;
	double dy = _vy * gm.last_dt;

	ent_msg msg;
	msg.type = ent_msg_t::MOVE;
	msg.move.dx = dx;
	msg.move.dy = dy;
	ent_msgs.push_back(msg);
}
