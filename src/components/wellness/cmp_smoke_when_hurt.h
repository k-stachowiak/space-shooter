#ifndef CMP_SMOKE_WHEN_HURT_H
#define CMP_SMOKE_WHEN_HURT_H

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

#include "../component.h"

class cmp_smoke_when_hurt : public component {

	default_random_engine _engine;

	// State.
	double _counter;

	// Trace location.
	double _last_health;
	double _max_health;
	double _last_x;
	double _last_y;

	void reset_counter(double rest);

public:
	cmp_smoke_when_hurt(uint32_t parent_id);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
