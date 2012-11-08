#ifndef CMP_EXPLODE_WHEN_DIE_H
#define CMP_EXPLODE_WHEN_DIE_H

#include "component.h"

class cmp_explode_when_die : public component {
	uint32_t _num_debris;
	double _last_x;
	double _last_y;
	double _last_dx;
	double _last_dy;
	bool _dead;
public:
	cmp_explode_when_die(uint32_t parent_id, uint32_t num_debris);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
