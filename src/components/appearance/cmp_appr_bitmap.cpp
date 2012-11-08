#include "cmp_appr_bitmap.h"

cmp_appr_bitmap::cmp_appr_bitmap(uint32_t parent_id, ALLEGRO_BITMAP* bitmap)
	: component(parent_id)
	, _bitmap(bitmap) {
}

void cmp_appr_bitmap::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	int w = al_get_bitmap_width(_bitmap);
	int h = al_get_bitmap_height(_bitmap);

	al_draw_rotated_bitmap(_bitmap, 
		(w >> 1), (h >> 1), 
		_last_x, _last_y, 
		_last_theta, 0);
}

void cmp_appr_bitmap::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
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
