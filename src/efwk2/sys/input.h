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

namespace efwk
{

template <class T>
using IsWeaponInputable = HasPlayerWeapons<T>;

template <class Entity>
typename std::enable_if<IsWeaponInputable<Entity>::value, void>::type
weapon_input(Entity& ent, const std::map<int, bool>& keys, double dt)
{
        auto& pweap = ent.pweap;

        pweap.update(dt);

        if (keys.at(ALLEGRO_KEY_Z)) {
                if (pweap.trigger_primary()) {
                        std::cout << "fire primary" << std::endl;
                }
        }

        if (keys.at(ALLEGRO_KEY_X)) {
                if (pweap.trigger_secondary()) {
                        std::cout << "fire secondary" << std::endl;
                }
        }
}

template <class Entity>
typename std::enable_if<!IsWeaponInputable<Entity>::value, void>::type
weapon_input(Entity&, const std::map<int, bool>&, double)
{
}

}

#endif
