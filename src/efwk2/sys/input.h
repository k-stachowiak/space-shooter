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

#ifndef INPUT_H
#define INPUT_H

#include <iostream>

#include "../../misc/config.h"
#include "../../resources/resman.h"

namespace efwk
{

template <class Entity>
void weapon_input_impl(Entity& ent,
                       const std::map<int, bool>& keys,
                       double dt,
                       comm_bus& cbus)
{
        auto& pweap = ent.pweap;
        const auto& ori = ent.ori;

        pweap.update(dt);

        if (keys.at(ALLEGRO_KEY_Z)) {
                if (pweap.trigger_primary()) {
                        double x, y;
                        std::tie(x, y) = ori.interpolate_loc(0);
                        cbus.bullet_reqs.push({ x, y, 0, -1, false, 10.0 });
                }
        }

        if (keys.at(ALLEGRO_KEY_X)) {
                if (pweap.trigger_secondary()) {
                        std::cout << "fire secondary" << std::endl;
                }
        }
}

template <class T>
using IsWeaponInputable = TmpAll<HasPlayerWeapons<T>, HasOrientation<T>>;

template <class Entity>
typename std::enable_if<IsWeaponInputable<Entity>::value, void>::type
weapon_input(Entity& ent,
             const std::map<int, bool>& keys,
             double dt,
             comm_bus& cbus)
{
        weapon_input_impl(ent, keys, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsWeaponInputable<Entity>::value, void>::type
weapon_input(Entity&, const std::map<int, bool>&, double, comm_bus&)
{}

}

#endif
