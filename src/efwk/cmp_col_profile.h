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

#ifndef CMP_COL_PROFILE_H
#define CMP_COL_PROFILE_H

#include <memory>
using std::shared_ptr;
using std::unique_ptr;

#include "../misc/maybe.h"
#include "cmp_misc.h"

namespace cmp {

class wellness;
class upgrades;

enum class pain_team {
        NONE,
        PLAYER,
        ENEMY
};

enum class coll_class {
        SHIP,
        PROJECTILE,
        PICKUP
};

class pickup_profile {
public:
        virtual ~pickup_profile() {}
        virtual bool trigger(
                wellness& w,
                upgrades& up,
                noise_queue& nqueue) = 0;
};

struct collision_profile {
        pain_team pt;
        coll_class cc;
        bool is_projectile;
        double dmg;
        unique_ptr<pickup_profile> pickup;
};

shared_ptr<collision_profile> create_collision_profile(
                pain_team pain_t,
                coll_class coll_c,
                bool is_projectile,
                double dmg,
                unique_ptr<pickup_profile> pick_p);

unique_ptr<pickup_profile> create_health_pickup_profile(double amount);
unique_ptr<pickup_profile> create_battery_pickup_profile(double amount);
unique_ptr<pickup_profile> create_bullet_upgrade_pickup_profile();
unique_ptr<pickup_profile> create_missile_upgrade_pickup_profile();

}

#endif
