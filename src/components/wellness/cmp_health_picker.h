#ifndef CMP_HEALTH_PICKER_H
#define CMP_HEALTH_PICKER_H

#include "../component.h"
#include "../../collision/collision_space.h"

class cmp_health_picker : public component {

	// Coniguration.
	double _health_boost;
	double _armor_boost;

	// State.
	vector<collision_class> _collisions;
public:
	cmp_health_picker(uint32_t parent_id, double health_boost, double armor_boost);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
