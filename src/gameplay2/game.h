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

#include "../resources/resman.h"

#include "../efwk2/comm.h"

#include "../efwk2/tmp/map.h"

#include "ent_player.h"
#include "ent_enemy.h"
#include "ent_bullet.h"

#include "../efwk2/sys/bounding.h"
#include "../efwk2/sys/display.h"
#include "../efwk2/sys/display_dbg.h"
#include "../efwk2/sys/movement.h"
#include "../efwk2/sys/input.h"
#include "../efwk2/sys/collision.h"
#include "../efwk2/sys/pain.h"

namespace gplay
{

class game
{
        const res::resman& m_resman;

        const double m_screen_w;
        const double m_screen_h;

        const std::map<int, bool>& m_keys;

        ALLEGRO_FONT* const m_debug_font;

        long m_next_id;

        double m_next_enemy_counter;

        player m_player;
        std::vector<bullet> m_bullets;
        std::vector<enemy> m_enemies;

        efwk::comm_bus m_cbus;

        long next_id() { return ++m_next_id; }

        void log_state();

        void update_entities(double dt);
        void handle_collisions();
        void handle_deletions(double dt);
        void handle_creations(double dt);

public:
        game(const res::resman& resman, const std::map<int, bool>& keys);
        void update(double dt);
        void draw(double weight);
};

}

#endif
