#ifndef CMP_SHOOT_PLAYER_H
#define CMP_SHOOT_PLAYER_H

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

#include "component.h"

class cmp_shoot_player : public component {

	default_random_engine _engine;

	double _accumulator;
	double _period;
	double _max_x_dev;

	// Was the previous shot from the left gun?
	bool _previous_left;

	double _last_x;
	double _last_y;

public:
	cmp_shoot_player(uint32_t parent_id, double period, double max_x_dev);

	void initialize(vector<ent_msg>& ent_msgs);
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif 
