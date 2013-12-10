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

#ifndef SYS_SOUND_H
#define SYS_SOUND_H

#include "../../resources/resman.h"
#include "../../misc/delq.h"
#include "base.h"
#include "nodes.h"

namespace sys {

class sound_system : public system {
        res::resman const& _resman;
        std::vector<nd::sound_node> _nodes;
        del_queue<res::res_id> _noise_queue;
public:
        sound_system(res::resman const& rm) : _resman(rm) {}
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        unsigned num_nodes() const { return _nodes.size(); }
        void add_node(nd::sound_node const& n) { _nodes.push_back(n); }
        void update(double dt);
};

}

#endif
