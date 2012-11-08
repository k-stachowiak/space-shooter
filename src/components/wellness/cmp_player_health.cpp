#include "cmp_player_health.h"

cmp_player_health::cmp_player_health(uint32_t parent_id)
: component(parent_id) {}

void cmp_player_health::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	gm.player_health = _last_health;
	gm.player_armor = _last_armor;
}

void cmp_player_health::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::HEALTH_ETC:
		_last_health = msg.health_etc.health;
		_last_armor = msg.health_etc.armor;
		break;

	default:
		break;
	}
}
