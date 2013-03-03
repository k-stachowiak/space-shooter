/* Copyright (C) 2012 Krzysztof Stachowiak */

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

#ifndef PATTERN_H
#define PATTERN_H

enum class enemy_type {
	light_fighter,
	heavy_fighter,
	light_bomber,
	heavy_bomber
};

enum class movement_type {
	zorro,
	diagonal,
	vertical,
	horizontal
};

struct pattern {

	struct element {
		double x_off;
		double y_off;
		enemy_type type;
	};

	vector<element> elements;
	movement_type movement;

	static vector<pattern::element> el_pair(enemy_type type) {
		return { { -70.0, 0.0, type },
				 {  70.0, 0.0, type } };
	}

	static vector<pattern::element> el_triangle(enemy_type type) {
		return { { -70.0, -40.0, type },
				 {  70.0, -40.0, type },
				 {   0.0,  30.0, type }};
	}

	static vector<pattern::element> el_quad(enemy_type type) {
		return { { -40.0, -35.0, type },
				 {  40.0, -35.0, type },
				 { -40.0,  35.0, type },
				 {  40.0,  35.0, type }};
	}
};

#endif
