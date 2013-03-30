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

#ifndef CMP_REACTION_H
#define CMP_REACTION_H

#include <memory>
using std::shared_ptr;

#include "comm.h"

namespace cmp {

class shape;

class reaction {
public:
	virtual ~reaction() {}
	virtual void trigger(
		double x, double y, double phi,
		shape const& shape,
		double vx, double vy,
		uint64_t origin_id,
                noise_queue& nqueue,
		comm::msg_queue& queue) = 0;
};

shared_ptr<reaction> create_complex_reaction(vector<shared_ptr<reaction>> rs);
shared_ptr<reaction> create_health_drop_reaction();
shared_ptr<reaction> create_battery_drop_reaction();
shared_ptr<reaction> create_bullet_upgrade_drop_reaction();
shared_ptr<reaction> create_missile_upgrade_drop_reaction();

shared_ptr<reaction> create_debris_reaction(
		uint32_t num_debris,
		vector<res_id> images,
		double vmin, double vmax,
		double theta_min, double theta_max,
		bool explode, bool randomize);

shared_ptr<reaction> create_explosion_sequence_reaction(
		uint32_t num_explosions,
		double min_delay,
		double max_delay);

}

#endif
