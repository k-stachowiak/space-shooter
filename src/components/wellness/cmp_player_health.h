#ifndef CMP_PLAYER_HEALTH_H
#define CMP_PLAYER_HEALTH_H

#include "../component.h"

class cmp_player_health : public component {
	double _last_health;
	double _last_armor;
public:
	cmp_player_health(uint32_t parent_id);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
