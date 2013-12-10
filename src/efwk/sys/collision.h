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

#ifndef SYS_COLLISION_H
#define SYS_COLLISION_H

#include <vector>

#include "../cmp/orientation.h"
#include "../cmp/col_profile.h"
#include "../cmp/pickup_profile.h"
#include "../cmp/shape.h"
#include "../cmp/coll_queue.h"

#include "base.h"

namespace sys {

struct collision_node {

        // origin_id   - points to the entity that has spawned this if it's a projectile
        // orientation - the offset of the shape
        // cp          - determines the profile of the object from the collision system pov
        // shape       - enables the collision tests
        // coll_queue  - stores the collisions from the given frame
        // name        - non-unique name for the debugging purposes

        uint64_t id;
        uint64_t origin_id;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::collision_profile> cp;
        std::shared_ptr<cmp::pickup_profile> pp;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::coll_queue> coll_queue;
        std::string name;
};

class collision_system : public updatable_system {
        std::vector<collision_node> _nodes;
        void check_collision(
                collision_node const& a,
                collision_node const& b) const;
public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(collision_node node) { _nodes.push_back(node); }
        void update(double dt, comm::msg_queue& msg);
};

}

#endif
