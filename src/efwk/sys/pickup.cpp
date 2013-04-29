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

#include "pickup.h"

namespace sys {

void pickup_system::update(comm::msg_queue& msgs) {
        for(auto const& n : _nodes) {
                n.coll_queue->for_each_report([&n, &msgs](cmp::coll_report const& r) {
                        if(r.cp->pickup) {
                                bool picked_up = r.cp->pickup->trigger(
                                                *(n.wellness),
                                                *(n.upgrades),
                                                *(n.nqueue));
                                if(picked_up) {
                                        msgs.push(comm::create_remove_entity(r.id));
                                }
                        }
                });
        }
}

}
