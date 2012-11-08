#ifndef CMP_PAINMAP_H
#define CMP_PAINMAP_H

#include <map>
using std::map;

#include "../component.h"
#include "../../collision/collision_space.h"

class cmp_painmap : public component {

	// Properties.
	map<collision_class, double> _pain_map;

	// State.
	vector<collision_class> _collisions;

public:
	cmp_painmap(uint32_t parent_id, map<collision_class, double> pain_map);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
