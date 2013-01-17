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

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <functional>
using std::function;

#include <algorithm>
using std::for_each;

#include <allegro5/allegro.h>

#include "../geometry/types.h"

namespace comm {
	class message;
}

namespace cmp {

// Shape base.
// -----------

class circle;

class shape {
public:
	virtual void shift(double dx, double dy) = 0;
	virtual void rotate(double dphi) = 0;
	virtual bool collides_with(shape const& shp) const = 0;
	virtual bool collides_with_circle(circle const& c) const = 0;
	virtual void debug_draw() {}
};

// Additional types.
// -----------------

// Collision class id.
enum class coll_class {
	PLAYER_SHIP,
	PLAYER_BULLET,
	PLAYER_MISSILE,
	ENEMY_SHIP,
	ENEMY_BULLET,
	ENEMY_MISSILE,
	HEALTH_PICKUP,
	ARMOR_PICKUP,
};

// Score class id.
enum class score_class {
	ENEMY_EYE,
	ENEMY_BOMBER
};

// Collision report type.
//
// Note: The pain id is the identifier of the node which
// deals damage through this node. E.g. If this node is a
// bullet, the pain_id will be that of the ship, who has
// launched the bullet. This way the entity responsible
// for the damage may be determined.
struct coll_report {

	// Store info about two colliding objects "a" and "b".
	struct {
		uint64_t id;
		uint64_t origin_id;
		coll_class cc;
		shared_ptr<shape> shp;
	} a, b;
};

// Animation frame definition.
struct frame_def {
	uint32_t index;
	double time;
	frame_def(uint32_t index, double time) : index(index), time(time) {}
	frame_def() : index(-1), time(-1) {}
	frame_def& operator=(const frame_def& rhs) {
		index = rhs.index;
		time = rhs.time;
		return *this;
	}
};

// Simple classes.
// ---------------

// The position and the rotation.
class orientation {
	double _x, _y, _phi;
public:
	orientation(double x, double y, double phi)
	: _x(x), _y(y), _phi(phi)
	{}

	double get_x() const { return _x; }
	double get_y() const { return _y; }
	double get_phi() const { return _phi; }
	void set_x(double x) { _x = x; }
	void set_y(double y) { _y = y; }
	void set_phi(double phi) { _phi = phi; }
};

// Universal type defining an AABB box.
class bounds {
	double _x_min;
	double _y_min;
	double _x_max;
	double _y_max;
public:
	bounds(double x_min, double y_min, double x_max, double y_max)
	: _x_min(x_min), _y_min(y_min), _x_max(x_max), _y_max(y_max)
	{}

	double get_x_min() const { return _x_min; }
	double get_y_min() const { return _y_min; }
	double get_x_max() const { return _x_max; }
	double get_y_max() const { return _y_max; }
};

// Collision queue.
class coll_queue {
	vector<coll_report> _queue;
public:
	void clear() { _queue.clear(); }
	void push_report(coll_report cr) { _queue.push_back(cr); }
	void for_each_report(function<void(coll_report const&)> f) {
		for_each(begin(_queue), end(_queue), f);
	}
};

// The mapping of the collision types to the amount of the
// damage they would deal upon a collision.
class painmap {
	map<coll_class, double> _pain_map;
public:
	painmap(map<coll_class, double> pain_map) : _pain_map(pain_map) {}
	double get_pain(coll_class const& cc) { 
		if(_pain_map.find(cc) == end(_pain_map))
			return 0.0;
		return _pain_map[cc];
	}
};

// The object's health armor etc.
class wellness {
	uint64_t _last_dmg_id; // NOTE that this is used to grant points for kill.
	double _max_health;
	double _health;
public:
	wellness(double health) : _max_health(health), _health(health) {}
	void deal_dmg(double dmg, uint64_t source_id) { _health -= dmg; _last_dmg_id = source_id; }
	double get_max_health() const { return _max_health; }
	double get_health() const { return _health; }
	bool is_alive() const { return _health > 0.0; }
	uint64_t get_last_dmg_id() const { return _last_dmg_id; }
};

// Timer base.
// -----------

class timer {
protected:
	uint32_t _ticks;
	double _counter;
public:
	virtual ~timer() {}
	uint32_t get_ticks() const { return _ticks; }
	void clear() { _ticks = 0; }
	void reset() { _ticks = _counter = 0; }
	virtual void update(double dt) = 0;
};

// Appearance base.
// ----------------

class appearance {
public:
	virtual ~appearance() {}
	virtual void update(double dt) = 0;
	virtual ALLEGRO_BITMAP* bitmap() const = 0;
};

// Dynamics base.
// --------------

class dynamics {
protected:
	double _vx;
	double _vy;
	double _theta;
public:
	dynamics() : _vx(0), _vy(0), _theta(0) {}
	virtual ~dynamics() {}
	virtual void update(double dt) = 0;
	double get_vx() const { return _vx; }
	double get_vy() const { return _vy; }
	double get_theta() const { return _theta; }
};

// Weapon behavior base.
// ---------------------

class weapon_beh {
public:
	virtual ~weapon_beh() {}
	virtual void update(
			uint64_t id,
			double dt,
			double x, double y,
			vector<comm::message>& msgs) = 0;
};

// FX base.
// --------

class fx {
public:
	virtual ~fx() {}
	virtual void update(
			double dt,
			double health_ratio,
			double x, double y,
			vector<comm::message>& msgs) = 0;
};

// Constructors.
// -------------

// Simple classes.

shared_ptr<orientation> create_orientation(double x, double y, double theta);

shared_ptr<bounds> create_bounds(double x_min, double y_min, double x_max, double y_max);

shared_ptr<coll_queue> create_coll_queue();

shared_ptr<painmap> create_painmap(map<coll_class, double> pain_map);

shared_ptr<wellness> create_wellness(double health);

// Timer classes.

shared_ptr<timer> create_const_int_timer(double interval);

// Appearance classes.

shared_ptr<appearance> create_static_bmp(ALLEGRO_BITMAP* bmp);

shared_ptr<appearance> create_simple_anim(
		ALLEGRO_BITMAP* bmp,
		uint32_t frame_width,
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int32_t rep_count);

// Dynamic classes.

shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy);

shared_ptr<dynamics> create_const_ang_vel_dynamics(double theta);

shared_ptr<dynamics> create_path_dynamics(vector<point> points);

// Shape classes.

shared_ptr<shape> create_circle(double x, double y, double r);

// Weapon behavior classes.

shared_ptr<weapon_beh> create_period_bullet(double dt_min, double dt_max);

shared_ptr<weapon_beh> create_period_missile(
		double dt_min, double dt_max,
		double x_off, double y_off);

// Fx classes.

shared_ptr<fx> create_smoke_when_hurt(double pain_threshold);

shared_ptr<fx> create_period_smoke(double dt_min, double dt_max);

}

#endif
