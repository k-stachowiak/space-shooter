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

#include "sys_fx.h"

namespace sys {

void fx_system::update(
		double dt,
		comm::msg_queue& msgs) {
	double max_health;
	double health;
	double x, y;
	for(auto const& n : _nodes) {
		x = n.orientation->get_x();
		y = n.orientation->get_y();
		max_health = n.wellness->get_max_health();
		health = n.wellness->get_health();
		n.effects->update(dt,
				health / max_health,
				x, y,
				n.shape,
				msgs);
	}
}

}
