#include "cmp_health_picker.h"

cmp_health_picker::cmp_health_picker(
	uint32_t parent_id,
	double health_boost,
	double armor_boost)
: component(parent_id)
, _health_boost(health_boost)
, _armor_boost(armor_boost)
{}

void cmp_health_picker::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {
	
	while(!_collisions.empty()) {

		collision_class cc = _collisions.back();
		_collisions.pop_back();

		if(cc == collision_class::HEALTH_PICKUP) {
			ent_msg msg;
			msg.type = ent_msg_t::PICKUP;
			msg.pickup.health = _health_boost;
			msg.pickup.armor = 0.0;
			ent_msgs.push_back(msg);

		} else if(cc == collision_class::ARMOR_PICKUP) {
			ent_msg msg;
			msg.type = ent_msg_t::PICKUP;
			msg.pickup.health = _armor_boost;
			msg.pickup.armor = 0.0;
			ent_msgs.push_back(msg);
		}
	}
}

void cmp_health_picker::receive_message(
		const ent_msg& msg,
		vector<ent_msg>& ent_msgs) {

	switch(msg.type) {
	case ent_msg_t::COLLISION:
		_collisions.push_back(msg.collision.cc);
		break;

	default:
		break;
	}
}
