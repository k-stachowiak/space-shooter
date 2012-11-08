#include "cmp_health_etc.h"

ent_msg cmp_health_etc::health_etc_msg() {
	ent_msg msg;
	msg.type = ent_msg_t::HEALTH_ETC;
	msg.health_etc.health = _health;
	msg.health_etc.armor = _armor;
	msg.health_etc.max_health = _max_health;
	msg.health_etc.max_armor = _max_armor;
	return msg;
}

cmp_health_etc::cmp_health_etc(uint32_t parent_id, double health, double armor)
	: component(parent_id)
	, _health(health)
	, _armor(armor)
	, _max_health(health)
	, _max_armor(armor) {
}

void cmp_health_etc::initialize(vector<ent_msg>& ent_msgs) {
	ent_msgs.push_back(health_etc_msg());
}

void cmp_health_etc::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	if(_health <= 0.0)
		gm.game_msgs.push_back(game_msg::create_remove_entity(_parent_id));
}

void cmp_health_etc::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {

	switch(msg.type) {
	case ent_msg_t::PAIN:
		_armor -= msg.pain.amount;
		if(_armor < 0) {
			_health += _armor; // NOTE: _armor is negative!
			_armor = 0;
		}
		ent_msgs.push_back(health_etc_msg());
		break;

	case ent_msg_t::PICKUP:

		_armor += msg.pickup.armor;
		if(_armor > _max_armor) _armor = _max_armor;

		_health += msg.pickup.health;
		if(_health > _max_health) _health = _max_health;

		ent_msgs.push_back(health_etc_msg());

		break;

	default:
		break;
	}
}
