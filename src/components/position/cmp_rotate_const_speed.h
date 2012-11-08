#ifndef CMP_ROTATE_CONST_SPEED_H
#define CMP_ROTATE_CONST_SPEED_H

#include "../component.h"

class cmp_rotate_const_speed : public component {
	double _omega;
public:
	cmp_rotate_const_speed(uint32_t parent_id, double omega);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {}
};

#endif
