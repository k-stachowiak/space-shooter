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

#include "wellness.h"

namespace {

        const double MAX_WEIGHT = 1.0;

}

namespace sys {

void wellness_system::update(double dt, comm::msg_queue& msgs) {

        for(auto const& n : _nodes) {

                bool died = false;
                if(n.wellness) {
                        if(!n.wellness->is_alive())
                                died = true;
                }

                if(n.ttl) {
                        n.ttl->update(dt);
                        if(n.ttl->get_ticks() > 0)
                                died = true;
                }

                if(died) {
                        double x, y;
                        double vx = 0;
                        double vy = 0;
                        if(n.dynamics) {
                                // Read the velocity.
                                vx = n.dynamics->get_vx();
                                vy = n.dynamics->get_vy();
                        }

                        // Handle reactions.
                        std::tie(x, y) = n.orientation->interpolate_loc(MAX_WEIGHT);
                        if(n.on_death)
                                n.on_death->trigger(
                                        x, y, n.orientation->get_phi(), *(n.shape),
                                        vx, vy, n.id, *(n.nqueue), msgs);

                        // Remove the entity.
                        msgs.push(comm::create_remove_entity(n.id));
                }
        }
}

}
