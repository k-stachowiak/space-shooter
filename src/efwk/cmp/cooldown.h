/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#ifndef COOLDOWN_H
#define COOLDOWN_H

#include <iostream>

namespace cmp {

    class cooldown {
        double _interval;
        double _counter;

    public:
        cooldown(double interval)
        : _interval(interval)
        , _counter(0.0)
        {}

        bool is_cooldown() const { return _counter > 0.0; }
        void reset() { if(_counter <= 0.0) _counter += _interval; }
        void update(double dt) { if(_counter > 0.0) _counter -= dt; }
    };

}

#endif

