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

#include "base.h"
#include "nodes.h"

namespace sys {

class collision_system : public system {
        template<typename SYS> friend void remove_node(SYS&, uint64_t);
        std::vector<nd::collision_node> _nodes;
        void check_collision(
                nd::collision_node const& a,
                nd::collision_node const& b) const;
public:
        unsigned num_nodes() const { return _nodes.size(); }
        void add_node(nd::collision_node node) { _nodes.push_back(node); }
        void update();
};

}

#endif
