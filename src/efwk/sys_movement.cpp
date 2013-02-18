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

template<class T>
inline bool between(T value, T min, T max) { return value >= min && value <= max; }

#include "sys_movement.h"

namespace sys {

void movement_system::update(
		double dt,
		comm::msg_queue& msgs) {

	for(auto const& n : _nodes) {

		// Determine velocities.
		// ---------------------
		double vx = 0;
		double vy = 0;
		double theta = 0;

		if(n.dynamics) {
			n.dynamics->update(dt);
			vx = n.dynamics->get_vx();
			vy = n.dynamics->get_vy();
			theta = n.dynamics->get_theta();
		}

		// Check bounds.
		// -------------
		double x = n.orientation->get_x();
		double y = n.orientation->get_y();
		double phi = n.orientation->get_phi();

		double dx = vx * dt;
		double dy = vy * dt;
		double dphi = theta * dt;

		double mini, maxi;

		if(n.movement_bounds) {
			mini = n.movement_bounds->get_x_min();
			maxi = n.movement_bounds->get_x_max();
			if(!between((x + dx), mini, maxi))
				dx = 0.0;

			mini = n.movement_bounds->get_y_min();
			maxi = n.movement_bounds->get_y_max();
			if(!between((y + dy), mini, maxi))
				dy = 0.0;

		}

		if(n.life_bounds) {
			mini = n.life_bounds->get_x_min();
			maxi = n.life_bounds->get_x_max();
			if(!between(x + dx, mini, maxi)) {
				msgs.push(comm::create_remove_entity(
							n.id));
				continue;
			}
			mini = n.life_bounds->get_x_min();
			maxi = n.life_bounds->get_y_max();
			if(!between(y + dy, mini, maxi)) {
				msgs.push(comm::create_remove_entity(
							n.id));
				continue;
			}
		}

		// Perform the actual move.
		// ------------------------
		n.orientation->set_x(x + dx);
		n.orientation->set_y(y + dy);
		n.orientation->set_phi(phi + dphi);

		if(n.shape) {
			n.shape->shift(dx, dy);
			n.shape->rotate(dphi);
		}
	}
}

}
