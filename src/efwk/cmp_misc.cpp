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

#include "cmp_misc.h"

namespace cmp {

shared_ptr<orientation> create_orientation(double x, double y, double theta) {
	return make_shared<orientation>(x, y, theta);
}

shared_ptr<bounds> create_bounds(
		double x_min, double y_min, double x_max, double y_max) {
	return make_shared<bounds>(x_min, y_min, x_max, y_max);
}

shared_ptr<coll_queue> create_coll_queue() {
	return make_shared<coll_queue>();
}

shared_ptr<ammo> create_ammo_unlimited() {
	return make_shared<ammo>(-1, -1);
}

shared_ptr<ammo> create_ammo(int bullets, int rockets) {
	return make_shared<ammo>(bullets, rockets);
}

shared_ptr<upgrades> create_upgrades() {
	return make_shared<upgrades>();
}

shared_ptr<wellness> create_wellness(double health) {
	return make_shared<wellness>(health);
}

}