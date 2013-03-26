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

	// sc       - tells how much points to grant
	// score	- the amount of the entitie's score.
	// wellness - tells if dead and who receives points

	uint64_t id;
	cmp::score_class sc;
	shared_ptr<double> score;
	shared_ptr<cmp::wellness> wellness;
};

struct input_node {

	// dynamics - allows for controlling the movement.

	uint64_t id;
	shared_ptr<cmp::dynamics> dynamics;
	shared_ptr<cmp::weapon_beh> weapon_beh;
};

struct drawing_node {

	// draw_plane  - determines the order in which to draw entities
	// appearance  - determines the bitmap to draw
	// orientation - determines to location and angle to draw at
	// shape       - allows for the debug drawing of the object's shape
	// pain_flash  - determines whether draw object or flash
	// dynamics    - allows for the debug print of the dynamical parameters

	uint64_t id;
	cmp::draw_plane draw_plane;
	shared_ptr<cmp::appearance> appearance;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<double> pain_flash;
	shared_ptr<cmp::dynamics> dynamics;
};

struct fx_node {

	// orientation - determines, where to draw the effect
	// shape       - enables picking a point from the shape
	// wellness    - for the wellness dependent effects
	// effects     - the fx object to be updated each frame

	uint64_t id;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::fx> effects;
};

struct movement_node {

	// dynamics        - determines the velocity of the entity in each frame
	// orientation     - is updated based on the current velocity
	// shape           - is updated based on the current velocity
	// movement_bounds - enables limiting the movement area of the entity
	// life_bounds     - enables killing an entity upon leaving the given area

	uint64_t id;
	shared_ptr<cmp::dynamics> dynamics;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::bounds> movement_bounds;
	shared_ptr<cmp::bounds> life_bounds;
};

struct arms_node {

	// orientation - determines the location base for spawning the bullets etc.
	// weapon_beh  - determines the projectiles spawning patterns
	// upgrades    - provides the information about the current player's upgrades

	uint64_t id;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::weapon_beh> weapon_beh;
	shared_ptr<cmp::upgrades> upgrades;
};

struct collision_node {

	// origin_id  - points to the entity that has spawned this if it's a projectile
	// cc         - determines the class of the object from the collision system pov
	// shape      - enables the collision tests
	// coll_queue - stores the collisions from the given frame

	uint64_t id;
	uint64_t origin_id;
	shared_ptr<cmp::collision_profile> cp;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::coll_queue> coll_queue;
};

struct pain_node {

	// coll_queue - contains candidates for the pain dealing
	// pain_map   - determines, how much damage should be dealt by given collision class
	// wellness   - is modified upon taking damage
	// pain_flash - to be modified upon hit

	uint64_t id;
	shared_ptr<cmp::coll_queue> coll_queue;
	shared_ptr<cmp::collision_profile> cp;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<double> pain_flash;
};

struct pickup_node {

	// coll_queue - contains candidates for the pickup
	// wellness   - to be modified if health picked up
	// ammo       - to be modified if ammo picked up
	// upgrades   - to be modified if upgrade picked up

	uint64_t id;
	shared_ptr<cmp::coll_queue> coll_queue;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::upgrades> upgrades;
};

struct wellness_node {

	// on_death    - the reaction to be triggered, when the entity dies
	// orientation - for the reaction
	// shape       - for the reaction
	// dynamics    - for the reaction
	// wellness    - monitored to notice, when the entity dies
	// ttl         - after the time to live runs out the entity dies

	uint64_t id;
	shared_ptr<cmp::reaction> on_death;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::dynamics> dynamics;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::timer> ttl;
};

struct hud_node {

	// score	- the value to be displayed
	// wellness	- for the health and shield display
	// upgrade	- the upgrades' status

	uint64_t id;
	shared_ptr<double> score;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::upgrades> upgrades;
};

}

#endif
