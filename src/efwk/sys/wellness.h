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

#ifndef SYS_WELLNESS_H
#define SYS_WELLNESS_H

#include <vector>
#include <memory>

#include "../cmp/reaction.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"
#include "../cmp/dynamics.h"
#include "../cmp/noise_queue.h"
#include "../cmp/wellness.h"
#include "../cmp/timer.h"

#include "../../misc/maybe.h"
#include "base.h"

namespace sys {

struct wellness_node {

        // on_death    - the reaction to be triggered, when the entity dies
        // orientation - for the reaction
        // shape       - for the reaction
        // dynamics    - for the reaction
        // nqueue      - for the reaction
        // wellness    - monitored to notice, when the entity dies
        // ttl         - after the time to live runs out the entity dies

        uint64_t id;
        std::shared_ptr<cmp::reaction> on_death;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::noise_queue> nqueue;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::timer> ttl;
};

class wellness_system : public updatable_system {
        std::vector<wellness_node> _nodes;

public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(wellness_node node) { _nodes.push_back(node); }
        void update(double dt, comm::msg_queue& msg);
};

}

#endif
