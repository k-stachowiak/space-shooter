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

#ifndef PICKUP_PROFILE_H
#define PICKUP_PROFILE_H

#include "noise_queue.h"

namespace cmp {

class wellness;
class upgrades;

struct pickup_profile {
        virtual ~pickup_profile() {}
        virtual bool trigger(
                wellness& w,
                upgrades& up,
                noise_queue& nqueue) = 0;
};

std::shared_ptr<pickup_profile> create_health_pickup_profile(double amount);
std::shared_ptr<pickup_profile> create_battery_pickup_profile(double amount);
std::shared_ptr<pickup_profile> create_bullet_upgrade_pickup_profile();
std::shared_ptr<pickup_profile> create_missile_upgrade_pickup_profile();

}

#endif
