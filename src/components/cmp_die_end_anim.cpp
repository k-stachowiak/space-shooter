#include "cmp_die_end_anim.h"

cmp_die_end_anim::cmp_die_end_anim(uint32_t parent_id)
	: component(parent_id)
	, _die(false) {
}

void cmp_die_end_anim::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	if(_die)
		gm.game_msgs.push_back(game_msg::create_remove_entity(_parent_id));
}

void cmp_die_end_anim::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ANIMATION_END_REACHED:
		_die = true;
		break;

	default:
		break;
	}
}
