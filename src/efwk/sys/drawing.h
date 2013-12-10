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

#include "base.h"
#include "nodes.h"

namespace sys {

class drawing_system : public system {
        std::map<cmp::draw_plane, std::vector<nd::drawing_node>> _nodes;
        ALLEGRO_FONT* _debug_font;
        void draw_plane(std::vector<nd::drawing_node> const& nodes, double weight);
public:

        void remove_node(uint64_t id)
        {
                for (auto& pr : _nodes)
                        remove_nodes(pr.second, id);
        }

        unsigned num_nodes() const {
                unsigned sizes = 0;
                for(auto const& pr : _nodes) {
                        sizes += pr.second.size();
                }
                return sizes;
        }

        drawing_system(ALLEGRO_FONT* debug_font) : _debug_font(debug_font) {}

        void add_node(nd::drawing_node n) { _nodes[n.draw_plane].push_back(n); }

        void update(double weight);

        friend void remove_node(drawing_system& sys, uint64_t id) {
                for(auto& pr : sys._nodes) {
                        for(auto n = begin(pr.second); n != end(pr.second); ++n) {
                                if(n->id == id) {
                                        *n = pr.second.back();
                                        pr.second.pop_back();
                                        --n;
                                }
                        }
                }
        }
};

}


#endif
