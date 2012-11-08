#ifndef CMP_APPR_PIXEL_H
#define CMP_APPR_PIXEL_H

#include "../component.h"

class cmp_appr_pixel : public component {
	double _r;
	double _g;
	double _b;
	double _a;

	double _last_x;
	double _last_y;

public:
	cmp_appr_pixel(uint32_t parent_id, double r, double g, double b, double a);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
