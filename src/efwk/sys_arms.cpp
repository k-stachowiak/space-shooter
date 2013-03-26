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

#include "sys_arms.h"

namespace sys {

void arms_system::update(double dt, comm::msg_queue& msgs) {
	double x, y;
	for(auto const& n : _nodes) {

		x = n.orientation->get_x();
		y = n.orientation->get_y();

		if(n.weapon_beh)
			n.weapon_beh->update(n.id, *(n.upgrades), dt, x, y, msgs);
	}
}

}
