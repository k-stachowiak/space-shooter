#ifndef CMP_ORIENTATION_H
#define CMP_ORIENTATION_H

#include "../component.h"

class cmp_orientation : public component {
	double _prev_x;
	double _prev_y;
	double _x;
	double _y;

	double _theta;

	ent_msg orientation_msg();

public:
	cmp_orientation(uint32_t parent_id, double x, double y, double theta);

	void initialize(vector<ent_msg>& ent_msgs);
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
