#ifndef CMP_MOVE_PATH_H
#define CMP_MOVE_PATH_H

#include "../../geometry/types.h"
#include "../component.h"

class cmp_move_path : public component {

	// Input.
	vector<point> _points;

	// State.
	bool _done;
	uint32_t _next_point;

	// Trace the position.
	double _last_x;
	double _last_y;

	ent_msg position_msg(double x, double y);

	void draw_debug();
	void perform_move(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);

public:
	cmp_move_path(uint32_t parent_id, vector<point> points);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
