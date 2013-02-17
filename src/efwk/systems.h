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

namespace sys {

// Helper components.
// ------------------

// System types declarations.
// --------------------------

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
			{ cmp::score_class::ENEMY_BOMBER, 5.0 },
			{ cmp::score_class::ENEMY_LIGHT_FIGHTER, 1.0 },
			{ cmp::score_class::ENEMY_HEAVY_FIGHTER, 3.0 },
			{ cmp::score_class::ENEMY_LIGHT_BOMBER, 5.0 },
			{ cmp::score_class::ENEMY_HEAVY_BOMBER, 7.0 }
		})
	{}

	void add_node(nd::score_node n) { _nodes.push_back(n); }
	void update();
	double get_score(uint64_t id) { return _ent_score_map[id]; }
};

class input_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	map<int, bool> _keys;
	vector<nd::input_node> _nodes;
public:
	void add_node(nd::input_node n) { _nodes.push_back(move(n)); }
	void update();
	void key_down(int k) { _keys[k] = true; }
	void key_up(int k) { _keys[k] = false; }
	bool pressed(int k) { return _keys[k]; }
};

class drawing_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	map<cmp::draw_plane, vector<nd::drawing_node>> _nodes;
	ALLEGRO_FONT* _debug_font;
	void draw_plane(double dt, vector<nd::drawing_node> const& nodes);
public:
	drawing_system(ALLEGRO_FONT* debug_font) : _debug_font(debug_font) {}
	void add_node(nd::drawing_node n) { _nodes[n.draw_plane].push_back(n); }
	void update(double dt);
	friend void remove_node(drawing_system& sys, uint64_t id) {
		for(auto& pr : sys._nodes) {
			for(auto n = begin(pr.second); n != end(pr.second); ++n) {
				if(n->id == id) {
					*n = pr.second.back();
					pr.second.pop_back();
					--n;
				}
			}
		}
	}
};

class fx_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::fx_node> _nodes;
public:
	void add_node(nd::fx_node n) { _nodes.push_back(n); }
	void update(double dt, comm::msg_queue& msgs);
};

class movement_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::movement_node> _nodes;
public:
	void add_node(nd::movement_node n) { _nodes.push_back(n); }
	void update(double dt, comm::msg_queue& msgs);
};

class arms_system : public system {

	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::arms_node> _nodes;

	uint64_t _tracked_id;
	shared_ptr<cmp::ammo> _tracked_ammo;

public:
	void add_node(nd::arms_node const& n) { _nodes.push_back(n); }

	void set_tracked_id(uint64_t tracked_id) { _tracked_id = tracked_id; }
	shared_ptr<cmp::ammo> get_tracked_ammo() { return _tracked_ammo; }

	void update(double dt, comm::msg_queue& msgs);
	void input(map<int, bool>& keys) {
		for(auto& n : _nodes) {
			n.weapon_beh->input(keys);
		}
	}
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
	void update(comm::msg_queue& msgs);
};

class pickup_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::pickup_node> _nodes;
public:
	void add_node(nd::pickup_node node) { _nodes.push_back(node); }
	void update(comm::msg_queue& msgs);
};

class wellness_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::wellness_node> _nodes;

	uint64_t _tracked_id;
	shared_ptr<cmp::wellness> _tracked_wellness;

public:
	void add_node(nd::wellness_node node) { _nodes.push_back(node); }
	void update(double dt, comm::msg_queue& msgs);

	void set_tracked_id(uint64_t tracked_id) { _tracked_id = tracked_id; }
	shared_ptr<cmp::wellness> get_tracked_wellness() { return _tracked_wellness; }
};

}

#endif
