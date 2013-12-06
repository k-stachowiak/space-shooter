/* Copyright (C) 2012, 2013 Krzysztof Stachowiak */

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

#ifndef WAVE_H
#define WAVE_H

#include <utility>

#include "../efwk/game_ent_fac.h"

#include "pattern.h"

struct spawn_desc {
        enemy_type type;
        double x, y;
        std::shared_ptr<cmp::dynamics> dynamics;
};

class wave {
        double _clock;
        std::vector<std::pair<double, pattern>> _patterns;
        unsigned _current_pattern;

public:
        wave(std::vector<std::pair<double, pattern>> patterns)
        : _clock(0) , _patterns(patterns) , _current_pattern(0) {}

        void reset() { _clock = _current_pattern = 0; }
        bool tick(double dt, entity_factory& ef, double screen_w, double screen_h);
};

#endif
