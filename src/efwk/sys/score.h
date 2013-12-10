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

#ifndef SYS_SCORE_H
#define SYS_SCORE_H

#include <map>

#include "../cmp/wellness.h"

#include "base.h"

namespace sys {

enum class score_class {
        PLAYER,
        ENEMY_LIGHT_FIGHTER,
        ENEMY_HEAVY_FIGHTER,
        ENEMY_LIGHT_BOMBER,
        ENEMY_HEAVY_BOMBER
};

struct score_node {

        // sc       - tells how much points to grant
        // score    - the amount of the entitie's score.
        // wellness - tells if dead and who receives points

        uint64_t id;
        score_class sc;
        std::shared_ptr<double> score;
        std::shared_ptr<cmp::wellness> wellness;
};


class score_system : public updatable_system {
        std::map<uint64_t, score_node> _nodes;
        const std::map<score_class, double> _class_score_map;
public:
        void remove_node(uint64_t id) { _nodes.erase(id); }

        score_system(std::map<score_class, double> score_map)
        : _class_score_map(score_map)
        {}

        void add_node(score_node n) { _nodes[n.id] = n; }
        void update(double dt, comm::msg_queue& msg);

        double get_score(uint64_t id) { return *(_nodes[id].score); }
};

}

#endif
