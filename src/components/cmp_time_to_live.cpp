#include "cmp_time_to_live.h"

cmp_time_to_live::cmp_time_to_live(uint32_t parent_id, double ttl)
	: component(parent_id)
	, _ttl(ttl) {
}

void cmp_time_to_live::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	_ttl -= gm.last_dt;
	if(_ttl <= 0)
		gm.game_msgs.push_back(game_msg::create_remove_entity(_parent_id));
}
