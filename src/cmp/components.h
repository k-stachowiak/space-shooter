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

namespace cmp {

// TODO: Move shapes and collision detection to another module.
// Shape base.
// -----------

class circle;

class shape {
public:
	virtual void shift(double dx, double dy) = 0;
	virtual bool collides_with(shape const& shp) const = 0;
	virtual bool collides_with_circle(circle const& c) const = 0;
};

// Complex types.
// --------------

// Collision class id.
enum class coll_class {
	PLAYER_SHIP,
	PLAYER_BULLET,
	ENEMY_SHIP,
	ENEMY_BULLET,
	HEALTH_PICKUP,
	ARMOR_PICKUP,
};

// Collision report type.
struct coll_report {
	coll_class cc_a;
	shared_ptr<shape> shp_a;
	coll_class cc_b;
	shared_ptr<shape> shp_b;
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

class orientation {
	double _x, _y, _theta;
public:
	orientation(double x, double y, double theta)
	: _x(x), _y(y), _theta(theta)
	{}

	double get_x() const { return _x; }
	double get_y() const { return _y; }
	double rotation() const { return _theta; }
	void set_x(double x) { _x = x; }
	void set_y(double y) { _y = y; }
};

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

class wellness {
	double _health;
public:
	wellness(double health) : _health(health) {}
	void deal_dmg(double dmg) { _health -= dmg; }
	double get_health() const { return _health; }
	bool is_alive() const { return _health > 0.0; }
};

// Appearance base.
// ----------------

class appearance {
public:
	virtual void update(double dt) = 0;
	virtual ALLEGRO_BITMAP* bitmap() const = 0;
};

// Dynamics base.
// --------------

class dynamics {
protected:
	double _vx;
	double _vy;
public:
	virtual void update(double dt) = 0;
	double get_vx() const { return _vx; }
	double get_vy() const { return _vy; }
};

// Collision queue related.
// ------------------------

class coll_queue {
	vector<coll_report> _queue;
public:
	void clear() { _queue.clear(); }
	void push_report(coll_report cr) { _queue.push_back(cr); }
	void for_each_report(function<void(coll_report const&)> f) {
		for_each(begin(_queue), end(_queue), f);
	}
};

// Constructors.
// -------------

shared_ptr<orientation> create_orientation(double x, double y, double theta);

shared_ptr<bounds> create_bounds(
		double x_min, double y_min, double x_max, double y_max);

shared_ptr<appearance> create_static_bmp(ALLEGRO_BITMAP* bmp);

shared_ptr<appearance> create_simple_anim(
		ALLEGRO_BITMAP* bmp,
		uint32_t frame_width,
		uint32_t num_frames,
		vector<frame_def> const& frame_defs,
		int32_t rep_count);

shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy);

shared_ptr<dynamics> create_path_dynamics(vector<point> points);

shared_ptr<shape> create_circle(double x, double y, double r);

shared_ptr<coll_queue> create_coll_queue();

shared_ptr<painmap> create_painmap(map<coll_class, double> pain_map);

shared_ptr<wellness> create_wellness(double health);

}

#endif
