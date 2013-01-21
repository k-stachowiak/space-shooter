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

#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <vector>
using std::vector;

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "nodes.h"
#include "comm.h"

template<class T>
inline bool between(T value, T min, T max) { return value >= min && value <= max; }

namespace sys {

template<typename SYS>
void remove_node(SYS& sys, uint64_t id) {
	for(auto n = begin(sys._nodes); n != end(sys._nodes); ++n) {
		if(n->id == id) {
			*n = sys._nodes.back();
			sys._nodes.pop_back();
			--n;
		}
	}
}

class system {
protected:
	bool _debug_mode;
public:
	void set_debug_mode(bool debug_mode) { _debug_mode = debug_mode; }
};

class score_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::score_node> _nodes;
	map<cmp::score_class, double> _class_score_map;
	map<uint64_t, double> _ent_score_map;
public:
	score_system()
	: _class_score_map(map<cmp::score_class, double> {
			{ cmp::score_class::ENEMY_EYE, 1.0 },
			{ cmp::score_class::ENEMY_BOMBER, 5.0 }
		})
	{}

	void add_node(nd::score_node n) { _nodes.push_back(n); }
	void update();
	double get_score(uint64_t id) { return _ent_score_map[id]; }
};

class drawing_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::drawing_node> _nodes;
	ALLEGRO_FONT* _debug_font;
public:
	drawing_system(ALLEGRO_FONT* debug_font) : _debug_font(debug_font) {}
	void add_node(nd::drawing_node n) { _nodes.push_back(n); }
	void update(double dt);
};

class fx_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::fx_node> _nodes;
public:
	void add_node(nd::fx_node n) { _nodes.push_back(n); }
	void update(double dt, vector<comm::message>& msgs);
};

class movement_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::movement_node> _nodes;
	uint64_t _player_controlled;
	double _player_throttle_x;
	double _player_throttle_y;
public:
	movement_system() : _player_throttle_x(0.0) , _player_throttle_y(0.0) {}
	void add_node(nd::movement_node n) { _nodes.push_back(n); }
	void set_player_controlled(uint64_t id) { _player_controlled = id; }
	void set_player_throttle(double x, double y) { _player_throttle_x = x; _player_throttle_y = y; }
	void update(double dt, vector<comm::message>& msgs);
};

class arms_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::arms_node> _nodes;
	uint64_t _player_shooting;
	double _player_interval;
	bool _player_trigger;
	double _player_counter;
	bool _player_prev_left;
	void handle_player(uint64_t id, double dt, vector<comm::message>& msgs, double x, double y);
	comm::message proc_msg(double x, double y, comm::message msg);
public:
	arms_system() : _player_counter(0.0) {}
	void add_node(nd::arms_node const& n) { _nodes.push_back(n); }
	void set_player_shooting(uint64_t id) { _player_shooting = id; }
	void set_player_interval(double interval) { _player_interval = interval; }
	void set_player_trigger(bool trigger) { _player_trigger = trigger; }
	void update(double dt, vector<comm::message>& msgs);
};

class collision_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::collision_node> _nodes;
	void check_collision(nd::collision_node const& a, nd::collision_node const& b);
public:
	void add_node(nd::collision_node node) { _nodes.push_back(node); }
	void update();
};

class pain_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::pain_node> _nodes;
public:
	void add_node(nd::pain_node node) { _nodes.push_back(node); }
	void update(vector<comm::message>& msgs);
};

class pickup_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::pickup_node> _nodes;
public:
	void add_node(nd::pickup_node node) { _nodes.push_back(node); }
	void update(vector<comm::message>& msgs);
};

class wellness_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	map<uint64_t, double> _entity_health_map;
	map<uint64_t, double> _entity_max_health_map;
	vector<nd::wellness_node> _nodes;
public:
	void add_node(nd::wellness_node node) { _nodes.push_back(node); }
	void update(double dt, vector<comm::message>& msgs);
	double get_ent_health(uint64_t id) { return _entity_health_map[id]; }
	double get_ent_health_ratio(uint64_t id) {
		return _entity_health_map[id] / _entity_max_health_map[id];
	}
};

}

#endif
