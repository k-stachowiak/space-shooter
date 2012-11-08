#ifndef CMP_HEALTH_ETC_H
#define CMP_HEALTH_ETC_H

#include "../component.h"

class cmp_health_etc : public component {
	double _health;
	double _armor;
	double _max_health;
	double _max_armor;

	ent_msg health_etc_msg();

public:
	cmp_health_etc(uint32_t parent_id, double health, double armor);

	void initialize(vector<ent_msg>& ent_msgs);
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
