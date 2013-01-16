/* Copyright (C) 2012 Krzysztof Stachowiak */

/*
* This file is part of space-shooter.
*
* space-shooter is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* space-shooter is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with space-shooter; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <array>
using std::array;

#include <string>
using std::string;

#include <random>
using std::uniform_real_distribution;

#include <allegro5/allegro_primitives.h>

#include "components.h"
#include "comm.h"
#include "../misc/rand.h"
#include "../geometry/misc.h"

namespace cmp {

// Timer classes.
// --------------

class const_int_timer : public timer {
	double _interval;
public:
	const_int_timer(double interval)
	: _interval(interval)
	{
		_counter = interval;
		_ticks = 0;
	}

	void update(double dt) {
		_counter -= dt;
		if(_counter < 0.0) {
			++_ticks;
			_counter += _interval;
		}
	}
};

// Appearance classes.
// -------------------

class static_bmp : public appearance {
	ALLEGRO_BITMAP* _bmp;
public:
	static_bmp(ALLEGRO_BITMAP* bmp) : _bmp(bmp) {}
	void update(double) {}
	ALLEGRO_BITMAP* bitmap() const { return _bmp; }
};

class simple_anim : public appearance {

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
	double _done;

public:
	simple_anim(
		ALLEGRO_BITMAP* bitmap, 
		uint32_t frame_width, 
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int rep_count)
	: _bitmap(bitmap)
	, _frame_width(frame_width)
	, _num_frames(num_frames)
	, _rep_count(rep_count)
	, _done(false) {

		// Generate the frame's sub-bitmaps
		for(uint32_t frame = 0; frame < _num_frames; ++frame)
			_frame_images[frame] = al_create_sub_bitmap(
				_bitmap,
				frame * _frame_width, 0,
				_frame_width, al_get_bitmap_height(_bitmap));

		// Store the definitions.
		_num_defs = frame_defs.size();
		for(uint32_t i = 0; i < _num_defs; ++i)
			_frame_defs[i] = frame_defs[i];

		// Setup the animation state.
		_current_def = 0;
		_time_to_switch = _frame_defs[_current_def].time;
	}

	~simple_anim() {
		for(uint32_t i = 0; i < _num_frames; ++i)
			al_destroy_bitmap(_frame_images[i]);
	}

	void update(double dt) {

		// Progress time.
		_time_to_switch -= dt;

		// Switch frame if necessary.
		if(!_done && (_time_to_switch <= 0.0)) {

			// Are we done?
			if(_current_def == _num_defs - 1) {
				if(_rep_count > 0)
					--_rep_count;

				if(_rep_count == 0) {
					_done = true;
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
	}

	ALLEGRO_BITMAP* bitmap() const {
		uint32_t current_index = _frame_defs[_current_def].index;
		return _frame_images[current_index];
	}

};

// Dynamics classes.
// -----------------

class const_velocity_dynamics : public dynamics {
public:
	const_velocity_dynamics(double vx, double vy) {
	       _vx = vx;
	       _vy = vy;
	}

	void update(double dt) {}
};

class const_ang_vel_dynamics : public dynamics {
public:
	const_ang_vel_dynamics(double theta) { _theta = theta; }
	void update(double dt) {}
};

class path_dynamics : public dynamics {
	// Configuration.
	vector<point> _points;

	// State.
	double _x, _y;
	uint32_t _next_point;
	bool _done;

public:
	path_dynamics(vector<point> points)
	: _points(points)
	, _x(points.front().x)
	, _y(points.front().y)
	, _next_point(1)
	, _done(false)
	{}

	void update(double dt) {

		if(_done)
			return;

		const double vel = 100.0;

		// Compute the translation towards next point.
		// -------------------------------------------

		// The direction derived from the current segment.
		double dir_x = _points[_next_point].x - _points[_next_point - 1].x;
		double dir_y = _points[_next_point].y - _points[_next_point - 1].y;
		double rsqrt = Q_rsqrt(dir_x * dir_x + dir_y * dir_y);

		// Compute the result velocity.
		_vx = dir_x * rsqrt * vel;
		_vy = dir_y * rsqrt * vel;

		// Compute the transiation for the given dt and analyze it.
		double dx = _vx * dt;
		double dy = _vy * dt;
		double new_x = _x + dx;
		double new_y = _y + dy;

		// Check if the next point has been stepped over.
		// ----------------------------------------------

		// Determine the vectors:
		// - towards the next point (n),
		// - towards the new position (p).
		double ndx = _points[_next_point].x - _x;
		double ndy = _points[_next_point].y - _y;
		double pdx = dx;
		double pdy = dy;

		// Detect moving on to another path's segment.
		// -------------------------------------------
		double dot = (ndx * pdx) + (ndy * pdy);
		if(dot < 0.0) {
			++_next_point;
			if(_next_point >= _points.size())
				_done = true;
		}

		// Store the new position.
		_x = new_x;
		_y = new_y;
	}

	double get_vx() const { return _vx; }
	double get_vy() const { return _vy; }
};

// Weapon behavior classes.
// ------------------------

class period_bullet : public weapon_beh {
	double _dt_min;
	double _dt_max;
	double _counter;

	void init_counter(double remainder = 0.0) {
		uniform_real_distribution<double> distr(_dt_min, _dt_max);
		_counter = distr(rnd::engine) - remainder;
	}

public:
	period_bullet(double dt_min, double dt_max)
	: _dt_min(dt_min)
	, _dt_max(dt_max)
       	, _counter(0) {
		init_counter();
	}

	void update(uint64_t id, double dt, double x, double y, vector<comm::message>& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push_back(comm::create_spawn_bullet(
						x, y,
						1.57, 0.0,
						800.0,
						true,
						id));
		}
	}
};

class period_missile : public weapon_beh {
	double _dt_min;
	double _dt_max;
	double _x_offset;
	double _y_offset;
	double _counter;

	void init_counter(double remainder = 0.0) {
		uniform_real_distribution<double> distr(_dt_min, _dt_max);
		_counter = distr(rnd::engine) - remainder;
	}

public:
	period_missile(double dt_min, double dt_max, double x_offset, double y_offset)
	: _dt_min(dt_min)
	, _dt_max(dt_max)
	, _x_offset(x_offset)
	, _y_offset(y_offset)
	, _counter(0) {
		init_counter();
	}

	void update(uint64_t id, double dt, double x, double y, vector<comm::message>& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push_back(comm::create_spawn_missile(
						x + _x_offset,
						y + _y_offset,
						1.57, 0.0,
						150.0,
						true,
						id));
		}
	}
};

// FX classes.
// -----------

class smoke_when_hurt : public fx {
	const_int_timer _timer;
	double _pain_threshold;
public:
	smoke_when_hurt(double pain_threshold)
	: _timer(0.125)
	, _pain_threshold(pain_threshold)
	{}

	void update(double dt, 
			double health_ratio,
			double x, double y,
			vector<comm::message>& msgs) {

		if(health_ratio > _pain_threshold)
			return;

		static uniform_real_distribution<double> dist(-20.0, 20.0);

		_timer.update(dt);
		for(uint32_t i = 0; i < _timer.get_ticks(); ++i) {
			double dx = dist(rnd::engine);
			double dy = dist(rnd::engine);
			msgs.push_back(comm::create_spawn_smoke(
						x + dx, y + dy,
						comm::smoke_size::medium));
		}
		_timer.clear();
	}
};

class period_smoke : public fx {
	double _dt_min;
	double _dt_max;
	double _counter;

	void init_counter(double remainder = 0.0) {
		uniform_real_distribution<double> distr(_dt_min, _dt_max);
		_counter = distr(rnd::engine) - remainder;
	}

public:
	period_smoke(double dt_min, double dt_max)
	: _dt_min(dt_min)
	, _dt_max(dt_max) {
		init_counter();
	}

	void update(	double dt,
			double health_ratio,
			double x, double y,
			vector<comm::message>& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push_back(comm::create_spawn_smoke(x, y, comm::smoke_size::small));
		}
	}
};

// Collision declarations.
// -----------------------

bool collide_circle_circle(const circle& a, const circle& b);

// Shape classes.
// --------------

class circle : public shape {
	double _x;
	double _y;
	double _r;
public:
	circle(double x, double y, double r)
	: _x(x), _y(y), _r(r) {}

	double get_x() const { return _x; }
	double get_y() const { return _y; }
	double get_r() const { return _r; }

	void shift(double dx, double dy) { _x += dx; _y += dy; }

	void rotate(double dphi) {}

	bool collides_with(const shape& s) const {
		return s.collides_with_circle(*this);
	}

	bool collides_with_circle(const circle& c) const {
		return collide_circle_circle(*this, c);
	}

	void debug_draw() {
		al_draw_circle(_x, _y, _r, al_map_rgb_f(1.0, 0.0, 0.0), 1.0);
	}
};

// Collision implementations.
// --------------------------

bool collide_circle_circle(const circle& a, const circle& b) {
	double dx = b.get_x() - a.get_x();
	double dy = b.get_y() - a.get_y();
	double length = 1.0 / Q_rsqrt(dx * dx + dy * dy);
	return length < (a.get_r() + b.get_r());
}

// Constructors.
// -------------

// Simple types.

shared_ptr<orientation> create_orientation(double x, double y, double theta) {
	return make_shared<orientation>(x, y, theta);
}

shared_ptr<bounds> create_bounds(
		double x_min, double y_min, double x_max, double y_max) {
	return make_shared<bounds>(x_min, y_min, x_max, y_max);
}

shared_ptr<coll_queue> create_coll_queue() {
	return make_shared<coll_queue>();
}

shared_ptr<painmap> create_painmap(map<coll_class, double> pain_map) {
	return make_shared<painmap>(pain_map);
}

shared_ptr<wellness> create_wellness(double health) {
	return make_shared<wellness>(health);
}

// Timer classes.

shared_ptr<timer> create_const_int_timer(double interval) {
	return shared_ptr<timer>(new const_int_timer(interval));
}

// Apperarance classes.

shared_ptr<appearance> create_static_bmp(ALLEGRO_BITMAP* bmp) {
	return shared_ptr<appearance>(new static_bmp(bmp));
}

shared_ptr<appearance> create_simple_anim(
		ALLEGRO_BITMAP* bmp,
		uint32_t frame_width,
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int32_t rep_count) {

	return shared_ptr<appearance>(new simple_anim(
			bmp,
			frame_width,
			num_frames,
			frame_defs,
			rep_count));
}

// Dynamic classes.

shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy) {
	return shared_ptr<dynamics>(new const_velocity_dynamics(vx, vy));
}

shared_ptr<dynamics> create_const_ang_vel_dynamics(double theta) {
	return shared_ptr<dynamics>(new const_ang_vel_dynamics(theta));
}

shared_ptr<dynamics> create_path_dynamics(vector<point> points) {
	return shared_ptr<dynamics>(new path_dynamics(points));
}

// Shape classes.

shared_ptr<shape> create_circle(double x, double y, double r) {
	return shared_ptr<shape>(new circle(x, y, r));
}

// Weapon behavior classes.

shared_ptr<weapon_beh> create_period_bullet(double dt_min, double dt_max) {
	return shared_ptr<weapon_beh>(new period_bullet(dt_min, dt_max));
}

shared_ptr<weapon_beh> create_period_missile(
		double dt_min,
		double dt_max,
		double x_off,
		double y_off) {
	return shared_ptr<weapon_beh>(new period_missile(
				dt_min, dt_max, x_off, y_off));
}

// Fx classes.

shared_ptr<fx> create_smoke_when_hurt(double pain_threshold) {
	return shared_ptr<fx>(new smoke_when_hurt(pain_threshold));
}

shared_ptr<fx> create_period_smoke(double dt_min, double dt_max) {
	return shared_ptr<fx>(new period_smoke(dt_min, dt_max));
}

}

