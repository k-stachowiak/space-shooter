#ifndef CMP_APPR_BITMAP_H
#define CMP_APPR_BITMAP_H

#include "../component.h"

#include <allegro5/allegro.h>

class cmp_appr_bitmap : public component {

	ALLEGRO_BITMAP* _bitmap;

	double _last_x;
	double _last_y;
	double _last_theta;

public:
	cmp_appr_bitmap(uint32_t parent_id, ALLEGRO_BITMAP* bitmap);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm) {}

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
