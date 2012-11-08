#ifndef CMP_SCORE_WHEN_DIE_H
#define CMP_SCORE_WHEN_DIE_H

#include "component.h"

class cmp_score_when_die : public component {
	double _score;
	bool _dead;
public:
	cmp_score_when_die(uint32_t parent_id, double score);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
