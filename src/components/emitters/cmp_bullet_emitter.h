#ifndef CMP_BULLET_EMITTER_H
#define CMP_BULLET_EMITTER_H

#include <random>
using namespace std;

#include "../component.h"

class cmp_bullet_emitter : public component {

	default_random_engine _engine;

	// Setup.
	double _dt_min;
	double _dt_max;
	double _vy;
	res_id _bitmap_id;
	bool _is_enemy;

	// State.
	double _counter;

	// Trace entity location.
	double _last_x;
	double _last_y;

	void reset_counter(double rest);

public:
	cmp_bullet_emitter(
		uint32_t parent_id,
		double dt_min, double dt_max,
		double vy,
		res_id bitmap_id, bool is_enemy);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
