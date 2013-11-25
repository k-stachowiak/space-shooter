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

#ifndef WEAPON_H
#define WEAPON_H

#include "cooldown.h"

namespace cmp {

    // Weapon's trigger component.
    class weapon {
        cooldown _cdown;
        bool _trigger;

    public:
        weapon(double interval)
        : _cdown(interval)
        , _trigger(false)
        {}

        void set_trigger(bool value) { _trigger = value; }

        bool update(double dt) {

            _cdown.update(dt);

            if(!_trigger || _cdown.is_cooldown()) {
                // Shooting not requested or not possible.
                return false;

            } else {
                // Shooting requested and possible.
                _cdown.reset();
                return true;

            }
        }
    };

}

#endif
