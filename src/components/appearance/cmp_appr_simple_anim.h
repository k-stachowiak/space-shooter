#ifndef CMP_APPR_SIMPLE_ANIM_H
#define CMP_APPR_SIMPLE_ANIM_H

#include <stdint.h>

#include <array>
using namespace std;

#include "../component.h"
#include "frame_def.h"

#include <allegro5/allegro.h>

class cmp_appr_simple_anim : public component {

	// The original bitmap - the frames' atlas.
	ALLEGRO_BITMAP* _bitmap;

	// The information about the particular frames.
	uint32_t _frame_width;
	uint32_t _num_frames;
	array<ALLEGRO_BITMAP*, 128> _frame_images;

	// The information about the frame definitions.
	uint32_t _num_defs;
	array<frame_def, 128> _frame_defs;

	// How many times should we repeat?
	int _rep_count;

	// The animation's state.
	uint32_t _current_def;
	double _time_to_switch;

	// Keeping track of the entitie's orientation.
	double _last_x;
	double _last_y;
	double _last_theta;
	double _done;

public:
	// negative rep_count means infinit loop
	cmp_appr_simple_anim(
		uint32_t parent_id,
		ALLEGRO_BITMAP* bitmap,
		uint32_t frame_width,
		uint32_t num_frames,
		vector<frame_def> frame_defs,
		int rep_count);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm);

	void action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
