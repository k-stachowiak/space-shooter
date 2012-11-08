#ifndef CMP_DIE_END_ANIM_H
#define CMP_DIE_END_ANIM_H

#include "component.h"

class cmp_die_end_anim : public component {
	bool _die;
public:
	cmp_die_end_anim(uint32_t parent_id);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
