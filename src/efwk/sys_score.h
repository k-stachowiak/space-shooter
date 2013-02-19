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

#ifndef SYS_SCORE_H
#define SYS_SCORE_H

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "sys_base.h"
#include "nodes.h"

namespace sys {

class score_system : public system {
	// TODO: implement a score component instead of the map here.
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

}

#endif
