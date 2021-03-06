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

#include "../cmp/components.h"

namespace nd {

/*
 * Note that the read only properties may be stored by value.
 * However all the other properties must be stored here as 
 * pointers as the changes made by one system must be visible
 * in another one.
 */

struct score_node {

        // sc       - tells how much points to grant
        // score    - the amount of the entitie's score.
        // wellness - tells if dead and who receives points

        uint64_t id;
        cmp::score_class sc;
        std::shared_ptr<double> score;
        std::shared_ptr<cmp::wellness> wellness;
};

struct input_node {

        // dynamics   - allows for controlling the movement.
        // weapon_beh - The weapon behavior to be triggered by the input.

        uint64_t id;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::weapon_beh> weapon_beh;
};

struct drawing_node {

        // draw_plane  - determines the order in which to draw entities
        // appearance  - determines the bitmap to draw
        // orientation - determines to location and angle to draw at
        // shape       - allows for the debug drawing of the object's shape
        // dynamics    - allows for the debug print of the dynamical parameters

        uint64_t id;
        cmp::draw_plane draw_plane;
        std::shared_ptr<cmp::appearance> appearance;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::dynamics> dynamics;
};

struct fx_node {

        // appearance  - The FX sys. is responsible for the updating of the appearance.
        // orientation - determines, where to draw the effect
        // shape       - enables picking a point from the shape
        // wellness    - for the wellness dependent effects
        // effects     - the fx object to be updated each frame

        uint64_t id;
        std::shared_ptr<cmp::appearance> appearance;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::fx> effects;
};

struct movement_node {

        // dynamics        - determines the velocity of the entity in each frame
        // orientation     - is updated based on the current velocity
        // shape           - is updated based on the current velocity
        // movement_bounds - enables limiting the movement area of the entity
        // life_bounds     - enables killing an entity upon leaving the given area

        uint64_t id;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::bounds> movement_bounds;
        std::shared_ptr<cmp::bounds> life_bounds;
};

struct arms_node {

        // orientation - determines the location base for spawning the bullets etc.
        // weapon_beh  - determines the projectiles spawning patterns
        // upgrades    - provides the information about the current player's upgrades
        // nqueue      - enables scheduling of the samples' playing

        uint64_t id;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::weapon_beh> weapon_beh;
        std::shared_ptr<cmp::upgrades> upgrades;
        std::shared_ptr<cmp::noise_queue> nqueue;
};

struct collision_node {

        // origin_id   - points to the entity that has spawned this if it's a projectile
        // orientation - the offset of the shape
        // cp          - determines the profile of the object from the collision system pov
        // shape       - enables the collision tests
        // coll_queue  - stores the collisions from the given frame
        // name        - non-unique name for the debugging purposes

        uint64_t id;
        uint64_t origin_id;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::collision_profile> cp;
        std::shared_ptr<cmp::pickup_profile> pp;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::coll_queue> coll_queue;
        std::string name;
};

struct pain_node {

        // coll_queue - contains candidates for the pain dealing
        // cp         - basis for the pain computation
        // wellness   - is modified upon taking damage

        uint64_t id;
        std::shared_ptr<cmp::coll_queue> coll_queue;
        std::shared_ptr<cmp::collision_profile> cp;
        std::shared_ptr<cmp::wellness> wellness;
};

struct pickup_node {

        // coll_queue - contains candidates for the pickup
        // wellness   - to be modified if health picked up
        // upgrades   - to be modified if upgrade picked up
        // nqueue     - to play sound upon pickup

        uint64_t id;
        std::shared_ptr<cmp::coll_queue> coll_queue;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::upgrades> upgrades;
        std::shared_ptr<cmp::noise_queue> nqueue;
};

struct wellness_node {

        // on_death    - the reaction to be triggered, when the entity dies
        // orientation - for the reaction
        // shape       - for the reaction
        // dynamics    - for the reaction
        // nqueue      - for the reaction
        // wellness    - monitored to notice, when the entity dies
        // ttl         - after the time to live runs out the entity dies

        uint64_t id;
        std::shared_ptr<cmp::reaction> on_death;
        std::shared_ptr<cmp::orientation> orientation;
        std::shared_ptr<cmp::shape> shape;
        std::shared_ptr<cmp::dynamics> dynamics;
        std::shared_ptr<cmp::noise_queue> nqueue;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::timer> ttl;
};

struct hud_node {

        // score    - the value to be displayed
        // wellness - for the health and shield display
        // upgrade  - the upgrades' status

        uint64_t id;
        std::shared_ptr<double> score;
        std::shared_ptr<cmp::wellness> wellness;
        std::shared_ptr<cmp::upgrades> upgrades;
};

struct sound_node {

        // nqueue - the queue of the noises to be played.

        uint64_t id;
        std::shared_ptr<cmp::noise_queue> nqueue;
};

}

#endif
