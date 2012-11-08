#ifndef CMP_MOVE_CONST_SPEED_H
#define CMP_MOVE_CONST_SPEED_H

#include "../component.h"

class cmp_move_const_speed : public component {
	double _vx;
	double _vy;

public:
	cmp_move_const_speed(uint32_t parent_id, double vx, double vy);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {}
};

#endif
