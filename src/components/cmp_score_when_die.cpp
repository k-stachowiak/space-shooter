#include "cmp_score_when_die.h"

cmp_score_when_die::cmp_score_when_die(uint32_t parent_id, double score)
: component(parent_id)
, _score(score)
, _dead(false)
{}

void cmp_score_when_die::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	if(_dead)
		gm.score += _score;
}

void cmp_score_when_die::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::HEALTH_ETC:
		if(msg.health_etc.health <= 0.0)
			_dead = true;
		break;

	default:
		break;
	}
}

