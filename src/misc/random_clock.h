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

#ifndef RANDOM_CLOCK_H
#define RANDOM_CLOCK_H

#include <random>
using std::default_random_engine;

#include <functional>
using std::function;

template<class DISTRIBUTION>
class random_clock {

        default_random_engine _engine;
        DISTRIBUTION _distribution;
        function<void()> _callback;
        double _timer;

        void schedule(double remainder) {
                _timer = _distribution(_engine);
                _timer -= remainder;
        }

public:
        random_clock(const DISTRIBUTION& distribution, function<void()> callback)
                : _distribution(distribution)
                , _callback(callback) {
                        schedule(0.0);
        }

        void tick(double dt) {
                _timer -= dt;
                while(_timer <= 0.0) {
                        schedule(-_timer);
                        _callback();
                }
        }
};

#endif
