/* Copyright (C) 2012,2013 Krzysztof Stachowiak */

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

#include <cmath>

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

// Reaction classes.
// -----------------

class complex_reaction : public reaction {
	vector<shared_ptr<reaction>> _rs;
public:
	complex_reaction(vector<shared_ptr<reaction>> rs) : _rs(rs) {}
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		for(auto r : _rs)
			r->trigger(x, y, phi, shape, vx, vy, origin_id, queue);
	}
};

class health_drop_reaction : public reaction {
public:
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		queue.push(comm::create_spawn_health_pickup(x, y, vx, vy));
	}
};

class missile_drop_reaction : public reaction {
public:
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		queue.push(comm::create_spawn_missiles_pickup(x, y, vx, vy));
	}
};

class debris_reaction : public reaction {
	uint32_t _num_debris;
	vector<res_id> _images;
	double _vmin, _vmax;
	double _theta_min, _theta_max;
	bool _explode;
	bool _randomize;
	
public:
	debris_reaction(uint32_t num_debris,
			vector<res_id> images,
			double vmin, double vmax,
			double theta_min, double theta_max,
			bool explode,
			bool randomize)
	: _num_debris(num_debris)
	, _images(images)
	, _vmin(vmin)
	, _vmax(vmax)
	, _theta_min(theta_min)
	, _theta_max(theta_max)
	, _explode(explode)
	, _randomize(randomize) 
	{}

	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {

		uniform_int_distribution<int> bmp_dist(0, _images.size() - 1);
		for(uint32_t i = 0; i < _num_debris; ++i) {
			uint32_t index = _randomize
				? bmp_dist(rnd::engine)
				: (i % _images.size());
			res_id bmp = _images[index];
			double deb_x, deb_y;
			tie(deb_x, deb_y) = shape->get_random_point();
			queue.push(comm::create_spawn_debris(
						deb_x, deb_y,
						vx, vy,
						_vmin, _vmax,
						_theta_min, _theta_max,
						bmp,
						_explode,
						origin_id));
		}
	}
};

class explosion_sequence_reaction : public reaction {
	uint32_t _num_explosions;
public:
	explosion_sequence_reaction(uint32_t num_explosions)
	: _num_explosions(num_explosions)
	{}

	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {

		double delay = 0.0;
		uniform_real_distribution<double> delay_dist(0.125, 0.25);
		for(uint32_t i = 0; i < _num_explosions; ++i) {
			double expl_x, expl_y;
			tie(expl_x, expl_y) = shape->get_random_point();
			queue.push(comm::create_spawn_explosion(expl_x, expl_y), delay);
			delay += delay_dist(rnd::engine);
		}
	}
};

// Appearance classes.
// -------------------

class pixel : public appearance {
	double _r, _g, _b;
public:
	pixel(double r, double g, double b) : _r(r), _g(g), _b(b) {}

	void update(double dt) {}

	void draw(double x, double y, double phi) const {
		al_draw_pixel(x + 0.5, y + 0.5, al_map_rgb_f(_r, _g, _b));
	}

	void draw_flash(double x, double y, double phi) const {}
};

class static_bmp : public appearance {
	ALLEGRO_BITMAP* _bmp;
	ALLEGRO_BITMAP* _flsh;
public:
	static_bmp(ALLEGRO_BITMAP* bmp, ALLEGRO_BITMAP* flsh) : _bmp(bmp), _flsh(flsh) {}
	void update(double) {}

	void draw(double x, double y, double phi) const {
		int w = al_get_bitmap_width(_bmp);
		int h = al_get_bitmap_height(_bmp);
		al_draw_rotated_bitmap(_bmp, 
				w / 2, h / 2,
				x, y, phi, 0);
	}

	void draw_flash(double x, double y, double phi) const {
		int w = al_get_bitmap_width(_flsh);
		int h = al_get_bitmap_height(_flsh);
		al_draw_rotated_bitmap(_flsh, 
				w / 2, h / 2,
				x, y, phi, 0);
	}
};

class simple_anim : public appearance {

	// The original bitmap - the frames' atlas.
	ALLEGRO_BITMAP* _bitmap;

	// The flashed variant of the bitmap.
	ALLEGRO_BITMAP* _flash;

	// The information about the particular frames.
	uint32_t _frame_width;
	uint32_t _num_frames;
	array<ALLEGRO_BITMAP*, 128> _frame_images;
	array<ALLEGRO_BITMAP*, 128> _flash_images;

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
		ALLEGRO_BITMAP* flash, 
		uint32_t frame_width, 
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int rep_count)
	: _bitmap(bitmap)
	, _flash(flash)
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
			
			_flash_images[frame] = al_create_sub_bitmap(
				_flash,
				frame * _frame_width, 0,
				_frame_width, al_get_bitmap_height(_flash));
		}

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

	void draw(double x, double y, double phi) const {
		uint32_t current_index = _frame_defs[_current_def].index;
		ALLEGRO_BITMAP* bmp = _frame_images[current_index];
		int w = al_get_bitmap_width(bmp);
		int h = al_get_bitmap_height(bmp);
		al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
	}

	void draw_flash(double x, double y, double phi) const {
		uint32_t current_index = _frame_defs[_current_def].index;
		ALLEGRO_BITMAP* bmp = _flash_images[current_index];
		int w = al_get_bitmap_width(bmp);
		int h = al_get_bitmap_height(bmp);
		al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
	}

};

// Dynamics classes.
// -----------------

class complex_dynamics : public dynamics {
	vector<shared_ptr<dynamics>> _ds;
public:
	complex_dynamics(vector<shared_ptr<dynamics>> ds)
	: _ds(ds) {}

	void update(double dt) {
		_vx = _vy = _theta = 0.0;
		for(auto const& d : _ds) {
			d->update(dt);
			_vx += d->get_vx();
			_vy += d->get_vy();
			_theta += d->get_theta();
		}
	}

	void input(map<int, bool>& keys) {
		for(auto const& d : _ds) {
			d->input(keys);
		}
	}
};

class const_velocity_dynamics : public dynamics {
public:
	const_velocity_dynamics(double vx, double vy) {
	       _vx = vx;
	       _vy = vy;
	}

	void update(double dt) {}
	void input(map<int, bool>& keys) {}
};

class const_acc_dynamics : public dynamics {
	double _vx0, _vy0;
	double _ax, _ay;
	double _t;
public:
	const_acc_dynamics(double vx0, double vy0, double ax, double ay)
	: _vx0(vx0), _vy0(vy0), _ax(ax), _ay(ay), _t(0)
	{}

	void update(double dt) {
		_t += dt;
		_vx = _vx0 + _ax * _t;
		_vy = _vy0 + _ay * _t;
	}

	void input(map<int, bool>& keys) {}
};

class const_ang_vel_dynamics : public dynamics {
public:
	const_ang_vel_dynamics(double theta) { _theta = theta; }
	void update(double dt) {}
	void input(map<int, bool>& keys) {}
};

class player_controlled_dynamics : public dynamics {
	double _throttle_x;
	double _throttle_y;
public:
	player_controlled_dynamics()
	: _throttle_x(0.0)
	, _throttle_y(0.0)
	{}

	void update(double dt) {
		_vx = _throttle_x * 400.0;
		_vy = _throttle_y * 300.0;
	}

	void input(map<int, bool>& keys) {
		_throttle_x = 0.0;
	       	_throttle_y = 0.0;
		if(keys[ALLEGRO_KEY_RIGHT]) _throttle_x += 1.0;
		if(keys[ALLEGRO_KEY_LEFT]) _throttle_x -= 1.0;
		if(keys[ALLEGRO_KEY_DOWN]) _throttle_y += 1.0;
		if(keys[ALLEGRO_KEY_UP]) _throttle_y -= 1.0;
	}
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

	void input(map<int, bool>& keys) {}
};

// Weapon behavior classes.
// ------------------------

class complex_weapon_beh : public weapon_beh {
	vector<shared_ptr<weapon_beh>> _wbs;
public:
	complex_weapon_beh(vector<shared_ptr<weapon_beh>> wbs) : _wbs(wbs) {}

	void update(uint64_t id,
			shared_ptr<ammo> ammo,
			double dt,
			double x, double y,
			comm::msg_queue& msgs) {

		for(auto& wb : _wbs) {
			wb->update(id, ammo, dt, x, y, msgs);
		}
	}

	void input(map<int, bool>& keys) {
		for(auto& wb: _wbs) {
			wb->input(keys);
		}
	}
};

class period_bullet : public weapon_beh {
	double _dt_min;
	double _dt_max;
	double _x_off;
	double _y_off;
	double _counter;

	void init_counter(double remainder = 0.0) {
		uniform_real_distribution<double> distr(_dt_min, _dt_max);
		_counter = distr(rnd::engine) - remainder;
	}

public:
	period_bullet(double dt_min, double dt_max, double x_off, double y_off)
	: _dt_min(dt_min)
	, _dt_max(dt_max)
	, _x_off(x_off)
	, _y_off(y_off)
       	, _counter(0) {
		init_counter();
	}

	void update(uint64_t id,
			shared_ptr<ammo> ammo,
			double dt,
			double x,
			double y,
			comm::msg_queue& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push(comm::create_spawn_bullet(
						x + _x_off,
						y + _y_off,
						1.57, 0.0,
						500.0,
						true,
						id));
		}
	}

	void input(map<int, bool>& keys) {}
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

	void update(uint64_t id,
			shared_ptr<ammo> ammo,
			double dt,
			double x,
			double y,
			comm::msg_queue& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push(comm::create_spawn_missile(
						x + _x_offset,
						y + _y_offset,
						1.57, 0.0,
						150.0,
						true,
						id));
		}
	}

	void input(map<int, bool>& keys) {}
};

class player_controlled_weapon_beh : public weapon_beh {

	bool _prev_left;
	weapon _minigun;
	weapon _rpg;

public:
	player_controlled_weapon_beh()
	: _minigun(0.1)
	, _rpg(0.75)
	{}

	void update(uint64_t id,
			shared_ptr<ammo> ammo,
			double dt,
			double x,
			double y,
			comm::msg_queue& msgs) {

		// Minigun fire.
		// -------------
		if(_minigun.update(dt) && ammo->get_bullets() != 0) {
			ammo->add_bullets(-1);
			if(_prev_left) {
				_prev_left = false;
				msgs.push(comm::create_spawn_bullet(
						x + 15.0, y,
						-1.57, 0.0,
						-800.0,
						false,
						id));
			} else {
				_prev_left = true;
				msgs.push(comm::create_spawn_bullet(
						x - 15.0, y,
						-1.57, 0.0,
						-800.0,
						false,
						id));
			}
		}

		// Rocket launcher fire.
		// ---------------------
		if(_rpg.update(dt) && ammo->get_rockets() != 0) {
			ammo->add_rockets(-1);
			msgs.push(comm::create_spawn_missile(
					x + 25.0, y,
					-1.57, 0.0,
					-300.0,
					false,
					id));
			msgs.push(comm::create_spawn_missile(
					x - 25.0, y,
					-1.57, 0.0,
					-300.0,
					false,
					id));
		}
	}

	void input(map<int, bool>& keys) {
		_minigun.set_trigger(keys[ALLEGRO_KEY_Z]);
		_rpg.set_trigger(keys[ALLEGRO_KEY_X]);
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
			shared_ptr<cmp::shape> shape,
			comm::msg_queue& msgs) {

		if(health_ratio > _pain_threshold)
			return;

		_timer.update(dt);
		for(uint32_t i = 0; i < _timer.get_ticks(); ++i) {
			double rnd_x, rnd_y;
			tie(rnd_x, rnd_y) = shape->get_random_point();
			msgs.push(comm::create_spawn_smoke(rnd_x, rnd_y, comm::smoke_size::medium));
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
			shared_ptr<cmp::shape> shape,
			comm::msg_queue& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push(comm::create_spawn_smoke(x, y, comm::smoke_size::tiny));
		}
	}
};

// Collision components.
// ---------------------

class simple_damage_profile : public damage_profile {
	double _amount;
public:
	simple_damage_profile(double amount) : _amount(amount) {}
	double compute_pain(pain_profile) {
		return _amount;
	}
};

class health_pickup_profile : public pickup_profile {
	double _amount;
public:
	health_pickup_profile(double amount) : _amount(amount) {}
	bool trigger(shared_ptr<wellness> w, shared_ptr<ammo> a) {

		double h = w->get_health();
		double H = w->get_max_health();

		if(h >= H) return false;

		if(h + _amount >= H) w->add_health(H - h);
		else w->add_health(_amount);

		return true;
	}
};

class missiles_pickup_profile : public pickup_profile {
	double _amount;
public:
	missiles_pickup_profile(double amount) : _amount(amount) {}
	bool trigger(shared_ptr<wellness> w, shared_ptr<ammo> a) {
		a->add_rockets(_amount);
		return true;
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

	pair<double, double> get_random_point() const {
		uniform_real_distribution<double> dist;
		double t = 6.28 * dist(rnd::engine);
		double u = dist(rnd::engine) + dist(rnd::engine);
		double r = (u > 1.0) ? (2.0 - u) : u;
		return make_pair(_x + _r * r * cos(t),
				 _y + _r * r * sin(t));
	}

	void debug_draw() {
		al_draw_circle(_x, _y, _r, al_map_rgb_f(1.0, 0.0, 0.0), 1.0);
	}
};

class complex_shape : public shape {
	vector<shared_ptr<shape>> _shapes;
public:
	complex_shape(vector<shared_ptr<shape>> shapes) : _shapes(shapes) {}

	void shift(double dx, double dy) {
		for(auto& s : _shapes) {
			s->shift(dx, dy);
		}
	}
	
	void rotate(double) {
		// Rotation not implemented.
		// Should more sophisticated geometry be applied,
		// use a 3rd party library...
	}

	bool collides_with(shape const& shp) const {
		for(auto& s : _shapes) {
			if(s->collides_with(shp)) {
				return true;
			}
		}
		return false;
	}

	bool collides_with_circle(circle const& c) const {
		for(auto& s : _shapes) {
			if(s->collides_with_circle(c)) {
				return true;
			}
		}
		return false;
	}

	pair<double, double> get_random_point() const {
		uniform_int_distribution<size_t> dist(0, _shapes.size() - 1);
		return _shapes.at(dist(rnd::engine))->get_random_point();
	}

	void debug_draw() {
		for(auto& s : _shapes) {
			s->debug_draw();
		}
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

shared_ptr<reaction> create_complex_reaction(vector<shared_ptr<reaction>> rs) {
	return shared_ptr<reaction>(new complex_reaction(rs));
}

shared_ptr<reaction> create_health_drop_reaction() {
	return shared_ptr<reaction>(new health_drop_reaction);
}

shared_ptr<reaction> create_missile_drop_reaction() {
	return shared_ptr<reaction>(new missile_drop_reaction);
}

shared_ptr<reaction> create_debris_reaction(
		uint32_t num_debris,
		vector<res_id> images,
		double vmin, double vmax,
		double theta_min, double theta_max,
		bool explode, bool randomize) {
	return shared_ptr<reaction>(new debris_reaction(
				num_debris,
				images,
				vmin, vmax,
				theta_min, theta_max,
				explode,
				randomize));
}

shared_ptr<reaction> create_explosion_sequence_reaction(uint32_t num_explosions) {
	return shared_ptr<reaction>(new explosion_sequence_reaction(num_explosions));
}

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

shared_ptr<ammo> create_ammo_unlimited() {
	return make_shared<ammo>(-1, -1);
}

shared_ptr<ammo> create_ammo(int bullets, int rockets) {
	return make_shared<ammo>(bullets, rockets);
}

shared_ptr<upgrades> create_upgrades() {
	return make_shared<upgrades>();
}

shared_ptr<wellness> create_wellness(double health) {
	return make_shared<wellness>(health);
}

// Timer classes.

shared_ptr<timer> create_const_int_timer(double interval) {
	return shared_ptr<timer>(new const_int_timer(interval));
}

// Apperarance classes.

shared_ptr<appearance> create_pixel(double r, double g, double b) {
	return shared_ptr<appearance>(new pixel(r, g, b));
}

shared_ptr<appearance> create_static_bmp(
		ALLEGRO_BITMAP* bmp,
		ALLEGRO_BITMAP* flash) {
	return shared_ptr<appearance>(new static_bmp(bmp, flash));
}

shared_ptr<appearance> create_simple_anim(
		ALLEGRO_BITMAP* bmp,
		ALLEGRO_BITMAP* flash,
		uint32_t frame_width,
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int32_t rep_count) {

	return shared_ptr<appearance>(new simple_anim(
			bmp,
			flash,
			frame_width,
			num_frames,
			frame_defs,
			rep_count));
}

// Dynamic classes.

shared_ptr<dynamics> create_complex_dynamics(vector<shared_ptr<dynamics>> ds) {
	return shared_ptr<dynamics>(new complex_dynamics(ds));
}

shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy) {
	return shared_ptr<dynamics>(new const_velocity_dynamics(vx, vy));
}

shared_ptr<dynamics> create_const_acc_dynamics(double vx0, double vy0, double ax, double ay) {
	return shared_ptr<dynamics>(new const_acc_dynamics(vx0, vy0, ax, ay));
}

shared_ptr<dynamics> create_const_ang_vel_dynamics(double theta) {
	return shared_ptr<dynamics>(new const_ang_vel_dynamics(theta));
}

shared_ptr<dynamics> create_path_dynamics(vector<point> points) {
	return shared_ptr<dynamics>(new path_dynamics(points));
}

shared_ptr<dynamics> create_player_controlled_dynamics() {
	return shared_ptr<dynamics>(new player_controlled_dynamics);
}

// Shape classes.

shared_ptr<shape> create_circle(double x, double y, double r) {
	return shared_ptr<shape>(new circle(x, y, r));
}

shared_ptr<shape> create_complex_shape(vector<shared_ptr<shape>> shapes) {
	return shared_ptr<shape>(new complex_shape(shapes));
}

// Weapon behavior classes.

shared_ptr<weapon_beh> create_complex_weapon_beh(vector<shared_ptr<weapon_beh>> wbs) {
	return shared_ptr<weapon_beh>(new complex_weapon_beh(wbs));
}

shared_ptr<weapon_beh> create_period_bullet(
		double dt_min,
		double dt_max,
		double x_off,
		double y_off) {
	return shared_ptr<weapon_beh>(new period_bullet(
				dt_min, dt_max, x_off, y_off));
}

shared_ptr<weapon_beh> create_period_missile(
		double dt_min,
		double dt_max,
		double x_off,
		double y_off) {
	return shared_ptr<weapon_beh>(new period_missile(
				dt_min, dt_max, x_off, y_off));
}

shared_ptr<weapon_beh> create_player_controlled_weapon_beh() {
	return shared_ptr<weapon_beh>(new player_controlled_weapon_beh);
}

// Fx classes.

shared_ptr<fx> create_smoke_when_hurt(double pain_threshold) {
	return shared_ptr<fx>(new smoke_when_hurt(pain_threshold));
}

shared_ptr<fx> create_period_smoke(double dt_min, double dt_max) {
	return shared_ptr<fx>(new period_smoke(dt_min, dt_max));
}

// Collision classes.

shared_ptr<collision_profile> create_collision_profile(
		pain_team pain_t,
		pain_profile pain_p,
		unique_ptr<damage_profile> dmg_p,
		unique_ptr<pickup_profile> pick_p) {

	return shared_ptr<collision_profile>(new collision_profile { pain_t, pain_p, move(dmg_p), move(pick_p) });
}

unique_ptr<damage_profile> create_simple_damage_profile(double amount) {
	return unique_ptr<damage_profile>(new simple_damage_profile(amount));
}

unique_ptr<pickup_profile> create_health_pickup_profile(double amount) {
	return unique_ptr<pickup_profile>(new health_pickup_profile(amount));
}

unique_ptr<pickup_profile> create_missiles_pickup_profile(double amount) {
	return unique_ptr<pickup_profile>(new missiles_pickup_profile(amount));
}

}

