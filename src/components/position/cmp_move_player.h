#ifndef CMP_MOVE_PLAYER_H
#define CMP_MOVE_PLAYER_H

#include "../component.h"

class cmp_move_player : public component {
	double _last_x;
	double _last_y;

public:
	cmp_move_player(uint32_t parent_id);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
