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

#ifndef NODES_H
#define NODES_H

#include <memory>
using std::shared_ptr;

#include "components.h"

namespace nd {

/*
 * Note that the read only properties may be stored by value.
 * However all the other properties must be stored here as 
 * pointers as the changes made by one system must be visible
 * in another one.
 */

struct score_node {
	uint64_t id;
	cmp::score_class sc;
	shared_ptr<cmp::wellness> wellness;
};

struct drawing_node {
	uint64_t id;
	shared_ptr<cmp::appearance> appearance;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<double> pain_flash;
	vector<shared_ptr<cmp::dynamics>> dynamics;
};

struct fx_node {
	uint64_t id;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::wellness> wellness;
	vector<shared_ptr<cmp::fx>> effects;
};

struct movement_node {
	uint64_t id;
	vector<shared_ptr<cmp::dynamics>> dynamics;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::bounds> movement_bounds;
	shared_ptr<cmp::bounds> life_bounds;
};

struct arms_node {
	uint64_t id;
	shared_ptr<cmp::orientation> orientation;
	vector<shared_ptr<cmp::weapon_beh>> weapon_beh;
	shared_ptr<cmp::ammo> ammo;
};

struct collision_node {
	uint64_t id;
	uint64_t origin_id;
	cmp::coll_class cc;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::coll_queue> coll_queue;
};

struct pain_node {
	uint64_t id;
	shared_ptr<cmp::coll_queue> coll_queue;
	shared_ptr<cmp::painmap> painmap;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<double> pain_flash;
};

struct pickup_node {
	uint64_t id;
	shared_ptr<cmp::coll_queue> coll_queue;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::ammo> ammo;
};

struct wellness_node {
	uint64_t id;
	bool explodes;
	bool spawn_health;
	bool spawn_missiles;
	uint32_t num_debris;
	shared_ptr<cmp::orientation> orientation;
	vector<shared_ptr<cmp::dynamics>> dynamics;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::timer> ttl;
};

}

#endif
