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

#ifndef CMP_WEAPON_BEH_H
#define CMP_WEAPON_BEH_H

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "comm.h"

namespace cmp {

class weapon_beh {
public:
	virtual ~weapon_beh() {}

	virtual void update(
			uint64_t id,
			upgrades& up,
			double dt,
			double x, double y,
			comm::msg_queue& msgs) = 0;

	virtual void input(map<int, bool>& keys) = 0;
};

shared_ptr<weapon_beh> create_complex_weapon_beh(vector<shared_ptr<weapon_beh>> wbs);

shared_ptr<weapon_beh> create_period_bullet(
		double dt_min, double dt_max,
		double x_off, double y_off);

shared_ptr<weapon_beh> create_period_missile(
		double dt_min, double dt_max,
		double x_off, double y_off);

shared_ptr<weapon_beh> create_player_controlled_weapon_beh();

}

#endif
