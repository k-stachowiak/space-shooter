#ifndef CMP_BOUNDED_MOVEMENT_H
#define CMP_BOUNDED_MOVEMENT_H

#include "../component.h"

class cmp_bounded_movement : public component {

	double _x_min;
	double _x_max;
	double _y_min;
	double _y_max;

public:
	cmp_bounded_movement(
		uint32_t parent_id,
		double min_x,
		double max_x,
		double min_y,
		double max_y);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs) {}
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
