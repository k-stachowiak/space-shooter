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
using std::map;

#include "base.h"
#include "nodes.h"

namespace sys {

class score_system : public system {
        map<uint64_t, nd::score_node> _nodes;
        const map<cmp::score_class, double> _class_score_map;
public:
        unsigned num_nodes() const { return _nodes.size(); }
        score_system(map<cmp::score_class, double> score_map)
        : _class_score_map(score_map)
        {}

        void add_node(nd::score_node n) { _nodes[n.id] = n; }
        void update();

        friend void remove_node(score_system& sys, uint64_t id) {
                sys._nodes.erase(id);
        }
};

}

#endif
