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

#ifndef SYS_FX_H
#define SYS_FX_H

#include <vector>

#include "../cmp/appr.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"
#include "../cmp/wellness.h"
#include "../cmp/fx.h"

#include "base.h"

namespace sys {

struct fx_node {

        // appearance  - The FX sys. is responsible for the updating of the appearance.
        // orientation - determines, where to draw the effect
        // shape       - enables picking a point from the shape
        // wellness    - for the wellness dependent effects
        // effects     - the fx object to be updated each frame

        uint64_t id;
        std::shared_ptr<cmp::appearance> appearance;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::fx> effects;
};

class fx_system : public updatable_system {
        std::vector<fx_node> _nodes;
public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(fx_node n) { _nodes.push_back(n); }
        void update(double dt, comm::msg_queue& msg);
};

}

#endif
