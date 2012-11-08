#ifndef CMP_DROP_WHEN_DIE_H
#define CMP_DROP_WHEN_DIE_H

#include "component.h"

class cmp_drop_when_die : public component {
	drop_t _drop_type;
	double _last_x;
	double _last_y;
	double _last_dx;
	double _last_dy;
	bool _dead;
public:
	cmp_drop_when_die(uint32_t parent_id, drop_t drop_type);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
