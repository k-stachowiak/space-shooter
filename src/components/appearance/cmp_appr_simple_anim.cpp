#include "cmp_appr_simple_anim.h"

cmp_appr_simple_anim::cmp_appr_simple_anim(
	uint32_t parent_id, 
	ALLEGRO_BITMAP* bitmap, 
	uint32_t frame_width, 
	uint32_t num_frames,
	vector<frame_def> frame_defs,
	int rep_count)
: component(parent_id)
, _bitmap(bitmap)
, _frame_width(frame_width)
, _num_frames(num_frames)
, _rep_count(rep_count)
, _done(false) {

	// Generate the frame's sub-bitmaps
	for(uint32_t frame = 0; frame < _num_frames; ++frame) {
		_frame_images[frame] = al_create_sub_bitmap(
			_bitmap,
			frame * _frame_width, 0,
			_frame_width, al_get_bitmap_height(_bitmap));
	}

	// Store the definitions.
	_num_defs = frame_defs.size();
	for(uint32_t i = 0; i < _num_defs; ++i) {
		_frame_defs[i] = frame_defs[i];
	}

	// Setup the animation state.
	_current_def = 0;
	_time_to_switch = _frame_defs[_current_def].time;
}

void cmp_appr_simple_anim::deinitialize(game_model& gm) {
	for(uint32_t i = 0; i < _num_frames; ++i) {		
		al_destroy_bitmap(_frame_images[i]);
	}
}

void cmp_appr_simple_anim::action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) {

	// Progress time.
	_time_to_switch -= gm.last_dt;

	// Switch frame if necessary.
	if(!_done && _time_to_switch <= 0.0) {

		// Are we done?
		if(_current_def == _num_defs - 1) {
			if(_rep_count > 0) {
				--_rep_count;
			}

			if(_rep_count == 0) {
				_done = true;
				ent_msg msg;
				msg.type = ent_msg_t::ANIMATION_END_REACHED;
				ent_msgs.push_back(msg);
				return;
			}
		}

		// How much have wee exceeded the frame time?
		double rest = -_time_to_switch;

		_current_def = (_current_def + 1) % _num_defs;
		_time_to_switch = _frame_defs[_current_def].time;

		// Take the extended time into account.
		_time_to_switch -= rest;
	}

	// Draw the current frame.
	uint32_t current_index = _frame_defs[_current_def].index;
	ALLEGRO_BITMAP* bitmap = _frame_images[current_index];
	int w = al_get_bitmap_width(bitmap);
	int h = al_get_bitmap_height(bitmap);
	al_draw_rotated_bitmap(bitmap, (w >> 1), (h >> 1), _last_x, _last_y, _last_theta, 0);
}

void cmp_appr_simple_anim::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		_last_theta = msg.orientation.theta;
		break;
	default:
		break;
	}
}
