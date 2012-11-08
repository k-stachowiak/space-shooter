#include "cmp_rotate_const_speed.h"

cmp_rotate_const_speed::cmp_rotate_const_speed(uint32_t parent_id, double omega)
: component(parent_id)
, _omega(omega) {}

void cmp_rotate_const_speed::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {

	double dtheta = _omega * gm.last_dt;

	ent_msg msg;
	msg.type = ent_msg_t::ROTATE;
	msg.rotate.dtheta = dtheta;
	ent_msgs.push_back(msg);
}
