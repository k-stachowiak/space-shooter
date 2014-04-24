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

#include "pickup_profile.h"
#include "wellness.h"
#include "noise_queue.h"
#include "upgrades.h"

namespace cmp {

class battery_pickup_profile : public pickup_profile {
        double _amount;
public:
        battery_pickup_profile(double amount) : _amount(amount) {}

        bool trigger(wellness& w,
                        upgrades& ,
                        noise_queue& nqueue) {

                double s = w.get_shield();
                double S = w.get_max_shield();

                if(s >= S) return false;

                if(s + _amount >= S) w.add_shield(S - s);
                else w.add_shield(_amount);

                nqueue.push(res::res_id::WELLNESS_PICKUP);

                return true;
        }
};

std::shared_ptr<pickup_profile> create_battery_pickup_profile(double amount) {
        return std::shared_ptr<pickup_profile>(new battery_pickup_profile(amount));
}

class health_pickup_profile : public pickup_profile {
        double _amount;
public:
        health_pickup_profile(double amount) : _amount(amount) {}
        bool trigger(wellness& w,
                        upgrades&,
                        noise_queue& nqueue) {

                double h = w.get_health();
                double H = w.get_max_health();

                if(h >= H) return false;

                if(h + _amount >= H) w.add_health(H - h);
                else w.add_health(_amount);

                nqueue.push(res::res_id::WELLNESS_PICKUP);

                return true;
        }
};

std::shared_ptr<pickup_profile> create_health_pickup_profile(double amount) {
        return std::shared_ptr<pickup_profile>(new health_pickup_profile(amount));
}

class bullet_upgrade_pickup_profile : public pickup_profile {
public:
        bool trigger(wellness& w,
                        upgrades& up,
                        noise_queue& nqueue) {
                if(up.can_upgrade_gun()) {
                        up.upgrade_gun();
                        nqueue.push(res::res_id::WEAPON_PICKUP);
                        return true;
                } else {
                        return false;
                }
        }
};

std::shared_ptr<pickup_profile> create_bullet_upgrade_pickup_profile() {
        return std::shared_ptr<pickup_profile>(new bullet_upgrade_pickup_profile());
}

class missile_upgrade_pickup_profile : public pickup_profile {
public:
        bool trigger(wellness& w,
                        upgrades& up,
                        noise_queue& nqueue) {
                if(up.can_upgrade_rl()) {
                        up.upgrade_rl();
                        nqueue.push(res::res_id::WEAPON_PICKUP);
                        return true;
                } else {
                        return false;
                }
        }
};

std::shared_ptr<pickup_profile> create_missile_upgrade_pickup_profile() {
        return std::shared_ptr<pickup_profile>(new missile_upgrade_pickup_profile());
}

}

