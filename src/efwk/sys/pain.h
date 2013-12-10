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

#ifndef SYS_PAIN_H
#define SYS_PAIN_H

#include <vector>

#include "../cmp/coll_queue.h"
#include "../cmp/col_profile.h"
#include "../cmp/wellness.h"

#include "base.h"

namespace sys {

struct pain_node {

        // coll_queue - contains candidates for the pain dealing
        // cp         - basis for the pain computation
        // wellness   - is modified upon taking damage

        uint64_t id;
        std::shared_ptr<cmp::coll_queue> coll_queue;
        std::shared_ptr<cmp::collision_profile> cp;
        std::shared_ptr<cmp::wellness> wellness;
};

class pain_system : public updatable_system {
        std::vector<pain_node> _nodes;
public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(pain_node node) { _nodes.push_back(node); }
        void update(double dt, comm::msg_queue& msg);
};

}

#endif
