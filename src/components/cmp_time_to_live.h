#ifndef CMP_TIME_TO_LIVE_H
#define CMP_TIME_TO_LIVE_H

#include "component.h"

class cmp_time_to_live : public component {
	double _ttl;

public:
	cmp_time_to_live(uint32_t parent_id, double ttl);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {}
};

#endif
