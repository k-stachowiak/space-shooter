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

#ifndef SYS_MOVEMENT_H
#define SYS_MOVEMENT_H

#include <vector>

#include "../cmp/dynamics.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"
#include "../cmp/bounds.h"

#include "base.h"

namespace sys {

struct movement_node {

        // dynamics        - determines the velocity of the entity in each frame
        // orientation     - is updated based on the current velocity
        // shape           - is updated based on the current velocity
        // movement_bounds - enables limiting the movement area of the entity
        // life_bounds     - enables killing an entity upon leaving the given area

        uint64_t id;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::bounds> movement_bounds;
        std::shared_ptr<cmp::bounds> life_bounds;
};

class movement_system : public updatable_system {
        std::vector<movement_node> _nodes;
public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(movement_node n) { _nodes.push_back(n); }
        void update(double dt, comm::msg_queue& msg);
};

}

#endif
