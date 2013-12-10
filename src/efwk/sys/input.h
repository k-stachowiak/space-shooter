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
#include <map>
#include <utility>

#include "../cmp/dynamics.h"
#include "../cmp/weapon_beh.h"

#include "base.h"

namespace sys {

struct input_node {

        // dynamics   - allows for controlling the movement.
        // weapon_beh - The weapon behavior to be triggered by the input.

        uint64_t id;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::weapon_beh> weapon_beh;
};

class input_system : public updatable_system {
        std::map<int, bool> _keys;
        std::vector<input_node> _nodes;
public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(input_node n) { _nodes.push_back(std::move(n)); }
        void update(double dt, comm::msg_queue& msg);
        void key_down(int k) { _keys[k] = true; }
        void key_up(int k) { _keys[k] = false; }
        bool pressed(int k) { return _keys[k]; }
};

}

#endif
