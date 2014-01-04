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

#ifndef SYS_DRAWING_H
#define SYS_DRAWING_H

#include <vector>
#include <map>

#include "../cmp/appr.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"
#include "../cmp/dynamics.h"

#include "base.h"

namespace sys {

// The drawing order management.
enum class draw_plane {
        BACKGROUND,
        SHIPS,
        FX,
        PROJECTILES
};

struct drawing_node {

        // plane       - determines the order in which to draw entities
        // appearance  - determines the bitmap to draw
        // orientation - determines to location and angle to draw at
        // shape       - allows for the debug drawing of the object's shape
        // dynamics    - allows for the debug print of the dynamical parameters

        uint64_t id;
        draw_plane plane;
        std::shared_ptr<cmp::appearance> appearance;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::dynamics> dynamics;
};


class drawing_system : public drawable_system {
        std::map<draw_plane, std::vector<drawing_node>> _nodes;
        void draw_on_plane(std::vector<drawing_node> const& nodes, double weight);
public:

        void remove_node(uint64_t id)
        {
                for (auto& pr : _nodes)
                        remove_nodes(pr.second, id);
        }

        void add_node(drawing_node n) { _nodes[n.plane].push_back(n); }

        void draw(double weight);
};

}


#endif
