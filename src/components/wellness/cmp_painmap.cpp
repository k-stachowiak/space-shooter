#include "cmp_painmap.h"

cmp_painmap::cmp_painmap(uint32_t parent_id, map<collision_class, double> pain_map)
	: component(parent_id)
	, _pain_map(pain_map) {
}

void cmp_painmap::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	while(!_collisions.empty()) {

		collision_class cc = _collisions.back();
		_collisions.pop_back();

		if(_pain_map.find(cc) != end(_pain_map)) {
			ent_msg msg;
			msg.type = ent_msg_t::PAIN;
			msg.pain.amount = _pain_map[cc];
			ent_msgs.push_back(msg);
		}
	}
}

void cmp_painmap::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::COLLISION:
		_collisions.push_back(msg.collision.cc);
		break;

	default:
		break;
	}
}
