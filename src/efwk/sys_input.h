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

#ifndef SYS_INPUT_H
#define SYS_INPUT_H

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <utility>
using std::move;

#include "sys_base.h"
#include "nodes.h"

namespace sys {

class input_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	map<int, bool> _keys;
	vector<nd::input_node> _nodes;
public:
	unsigned num_nodes() const { return _nodes.size(); }
	void add_node(nd::input_node n) { _nodes.push_back(move(n)); }
	void update();
	void key_down(int k) { _keys[k] = true; }
	void key_up(int k) { _keys[k] = false; }
	bool pressed(int k) { return _keys[k]; }
};

}

#endif
