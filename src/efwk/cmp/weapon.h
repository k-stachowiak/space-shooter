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

namespace cmp {

    // Weapon's trigger component.
    class weapon {
            double _interval;
            bool _trigger;
            double _counter;

    public:
            weapon(double interval)
            : _interval(interval)
            , _trigger(false)
            , _counter(0.0)
            {}

            void set_trigger(bool value) { _trigger = value; }

            bool update(double dt) {
                    if(_counter > 0.0) _counter -= dt;
                    if(!_trigger || _counter > 0.0) return false;
                    _counter += _interval;
                    return true;
            }
    };

}

#endif
