#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>

#include "cmp_appr_pixel.h"

cmp_appr_pixel::cmp_appr_pixel(uint32_t parent_id, double r, double g, double b, double a) 
	: component(parent_id)
	, _r(r)
	, _g(g)
	, _b(b)
	, _a(a) {
}

void cmp_appr_pixel::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	al_put_pixel(_last_x, _last_y, al_map_rgba_f(_r, _g, _b, _a));
}

void cmp_appr_pixel::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;
	default:
		break;
	}
}
