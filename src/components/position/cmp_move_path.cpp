#include <allegro5/allegro_primitives.h>

#include "../../geometry/misc.h"

#include "cmp_move_path.h"

ent_msg cmp_move_path::position_msg(double x, double y) {

	double dx = x - _last_x;
	double dy = y - _last_y;

	ent_msg msg;
	msg.type = ent_msg_t::MOVE;
	msg.move.dx = dx;
	msg.move.dy = dy;

	return msg;
}

void cmp_move_path::draw_debug() {
	for(uint32_t i = 1; i < _points.size(); ++i) {
		double x1 = _points[i - 1].x;
		double y1 = _points[i - 1].y;
		double x2 = _points[i - 0].x;
		double y2 = _points[i - 0].y;
		al_draw_line(x1, y1, x2, y2,
			al_map_rgba_f(0.0f, 0.0f, 1.0f, 1.0f), 1);
	}
}

void cmp_move_path::perform_move(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	// Compute the translation towards next point.
	// -------------------------------------------

	// The direction derived from the current segment.
	double dir_x = _points[_next_point].x - _points[_next_point - 1].x;
	double dir_y = _points[_next_point].y - _points[_next_point - 1].y;

	// Normalize and scale to reflect requested velocity.
	double rsqrt = Q_rsqrt(dir_x * dir_x + dir_y * dir_y);
	double dx = dir_x * rsqrt * cfg.get_eye_v() * gm.last_dt;
	double dy = dir_y * rsqrt * cfg.get_eye_v() * gm.last_dt;

	// Compute the next position.
	double new_x = _last_x + dx;
	double new_y = _last_y + dy;

	// Check if the next point has been stepped over.
	// ----------------------------------------------

	// Determine the vectors towards the next point (n) and towards the new position (p).
	double ndx = _points[_next_point].x - _last_x;
	double ndy = _points[_next_point].y - _last_y;
	double pdx = dx;
	double pdy = dy;

	// Compute the dot product
	double dot = (ndx * pdx) + (ndy * pdy);

	// React based on the path traversal status.
	// -----------------------------------------

	if(dot > 0.0) {
		// The next point still ahead.
		ent_msgs.push_back(position_msg(new_x, new_y));

	} else {
		// The next point has been stepped over - update the state
		++_next_point;

		// Handle the end of the routine.
		if(_next_point >= _points.size()) {
			ent_msg msg;
			msg.type = ent_msg_t::PATH_END_REACHED;
			ent_msgs.push_back(msg);
			_done = true;
		}
	}
}

cmp_move_path::cmp_move_path(uint32_t parent_id, vector<point> points)
	: component(parent_id)
	, _points(points) 
	, _done(false)
	, _next_point(1) {
}

void cmp_move_path::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	if(_done)
		return;
	
	if(gm.debug)
		draw_debug();

	perform_move(cfg, gm, ent_msgs);
}

void cmp_move_path::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;

	default:
		break;
	}
}

