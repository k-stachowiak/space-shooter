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

#include "sys_collision.h"

namespace sys {

void collision_system::check_collision(nd::collision_node const& a,
                         nd::collision_node const& b) const {

        cmp::shape const& shp_a = *(a.shape);
        cmp::shape const& shp_b = *(b.shape);

        if(shp_a.collides_with(shp_b)) {
                a.coll_queue->push_report({ b.id, b.origin_id, b.cp, b.shape });
                b.coll_queue->push_report({ a.id, a.origin_id, a.cp, a.shape });
        }
}

void collision_system::update() {
        // Clear the collision queues first.
        for(auto a = begin(_nodes); a != end(_nodes); ++a)
                a->coll_queue->clear();

        // Perform the collision checks.
        for(auto a = begin(_nodes); a != end(_nodes); ++a)
                for(auto b = a + 1; b != end(_nodes); ++b)
                        check_collision(*a, *b);
}

}
