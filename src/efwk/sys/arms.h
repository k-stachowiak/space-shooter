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

#ifndef SYS_ARMS_H
#define SYS_ARMS_H

#include <vector>
#include <memory>
#include <map>

#include "../cmp/orientation.h"
#include "../cmp/weapon_beh.h"
#include "../cmp/upgrades.h"
#include "../cmp/noise_queue.h"

#include "../../misc/maybe.h"
#include "base.h"

namespace sys {

struct arms_node {

        // orientation - determines the location base for spawning the bullets etc.
        // weapon_beh  - determines the projectiles spawning patterns
        // upgrades    - provides the information about the current player's upgrades
        // nqueue      - enables scheduling of the samples' playing

        uint64_t id;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::weapon_beh> weapon_beh;
        std::shared_ptr<cmp::upgrades> upgrades;
        std::shared_ptr<cmp::noise_queue> nqueue;
};

class arms_system : public updatable_system {
        std::vector<arms_node> _nodes;

public:
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        void add_node(arms_node const& n) { _nodes.push_back(n); }
        void update(double dt, comm::msg_queue& msg);
        void input(std::map<int, bool>& keys) {
                for(auto& n : _nodes) {
                        n.weapon_beh->input(keys);
                }
        }
};

}

#endif
