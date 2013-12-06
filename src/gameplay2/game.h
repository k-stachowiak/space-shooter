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

#ifndef GAME_H
#define GAME_H

#include "../efwk2/ent/player_controlled.h"
#include "../efwk2/sys/bounding.h"
#include "../efwk2/sys/display.h"
#include "../efwk2/sys/movement.h"
#include "../efwk2/sys/input.h"

namespace gplay
{

class game
{
        const double m_screen_w;
        const double m_screen_h;

        const std::map<int, bool>& m_keys;

        efwk::player_controlled m_player;

public:
        game(const std::map<int, bool>& keys);
        void update(double dt);
        void draw(double weight);
};

}

#endif
