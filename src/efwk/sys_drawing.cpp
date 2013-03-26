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

#include "../misc/config.h"
#include "sys_drawing.h"

namespace sys {

void drawing_system::draw_plane(double dt, vector<nd::drawing_node> const& nodes) {

	double x, y;
	double phi;
	for(auto const& n : nodes) {

		n.appearance->update(dt);

		x = n.orientation->get_x();
		y = n.orientation->get_y();
		phi = n.orientation->get_phi();

		if(*(n.pain_flash) > 0.0) {
			n.appearance->draw_flash(x, y, phi);
			*(n.pain_flash) -= dt;
		} else {
			n.appearance->draw(x, y, phi);
		}

		if(_debug_mode) {
			if(n.shape) {
				n.shape->debug_draw();
			}
		}
	}
}

void drawing_system::update(double dt) {

	al_clear_to_color(al_map_rgb_f(
			cfg::real("gfx_background_r"),
			cfg::real("gfx_background_g"),
			cfg::real("gfx_background_b")));

	draw_plane(dt, _nodes[cmp::draw_plane::BACKGROUND]);
	draw_plane(dt, _nodes[cmp::draw_plane::SHIPS]);
	draw_plane(dt, _nodes[cmp::draw_plane::PROJECTILES]);
	draw_plane(dt, _nodes[cmp::draw_plane::FX]);
}

}
