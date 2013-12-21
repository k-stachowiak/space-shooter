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

#include "../efwk2/ent/player_controlled.h"
#include "../efwk2/ent/enemy.h"
#include "../efwk2/ent/bullet.h"

#include "../efwk2/sys/bounding.h"
#include "../efwk2/sys/display.h"
#include "../efwk2/sys/movement.h"
#include "../efwk2/sys/input.h"
#include "../efwk2/sys/collision.h"

namespace gplay
{

class game
{
        const res::resman& m_resman;

        const double m_screen_w;
        const double m_screen_h;

        const std::map<int, bool>& m_keys;

        long m_next_id;

        double m_next_enemy_counter;

        efwk::player_controlled m_player;
        std::vector<efwk::bullet> m_bullets;
        std::vector<efwk::enemy> m_enemies;

        efwk::comm_bus m_cbus;

        long next_id() { return ++m_next_id; }

        void spawn_enemy_process(double dt);

        template <class Entity>
        void update_ent(Entity& ent, double dt)
        {
                efwk::weapon_input(ent, m_keys, dt, m_resman, m_cbus);
                efwk::move(ent, dt);
                efwk::bind_movement(ent);
                efwk::bind_life(ent, m_cbus);
        }

        template <class Entity>
        bool try_remove_ent(std::vector<Entity>& v, long rem_id)
        {
                auto found = std::find_if(
                        begin(v), end(v),
                        [rem_id](const Entity& e) {
                                return e.id == rem_id;
                        });

                if (found != end(v)) {
                        *found = std::move(v.back());
                        v.pop_back();
                        return true;
                }

                return false;
        }

public:
        game(const res::resman& resman, const std::map<int, bool>& keys);
        void update(double dt);
        void draw(double weight);
};

}

#endif
