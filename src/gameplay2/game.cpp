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

#include "game.h"

#include "../misc/config.h"

namespace gplay
{

void game::spawn_enemy_process(double dt)
{
        if ((m_next_enemy_counter -= dt) > 0) {
                return;
        }

        m_next_enemy_counter += 3.0;
        m_enemies.emplace_back(
                next_id(),
                m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER),
                100.0,
                400.0, 10.0,
                0, 0, 
                cfg::integer("gfx_screen_w"),
                cfg::integer("gfx_screen_h"));
}

game::game(const res::resman& resman, const std::map<int, bool>& keys) :
        m_resman(resman),
        m_screen_w(cfg::integer("gfx_screen_w")),
        m_screen_h(cfg::integer("gfx_screen_h")),
        m_keys(keys),
        m_next_id(0),
        m_next_enemy_counter(0),
        m_player(next_id(),
                m_resman.get_bitmap(res::res_id::PLAYER_SHIP),
                400.0, m_keys,
                100.0, 100.0, -3.1415 * 0.5,
                0.0, 0.0, m_screen_w, m_screen_h,
                0.1, 1.0)
{
}

void game::update(double dt)
{
        // Update entities.

        efwk::weapon_input(m_player, m_keys, dt, m_resman, m_cbus);
        efwk::move_ent(m_player, dt);
        efwk::bind_movement(m_player);

        std::for_each(begin(m_bullets), end(m_bullets), [dt, this](efwk::bullet& b) {
                efwk::move_ent(b, dt);
                efwk::bind_life(b, m_cbus);
        });

        std::for_each(begin(m_enemies), end(m_enemies), [dt, this](efwk::enemy& e) {
                efwk::move_ent(e, dt);
                efwk::bind_life(e, m_cbus);
        });

        // Handle deletion messages.

        m_cbus.dels.visit(dt, [this](long rem_id) {
                auto found_bullet = std::find_if(begin(m_bullets), end(m_bullets),
                        [rem_id](const efwk::bullet& b) {
                                return b.id == rem_id;
                        });

                if (found_bullet != end(m_bullets)) {
                        *found_bullet = std::move(m_bullets.back());
                        m_bullets.pop_back();
                        return;
                }

                auto found_enemy = std::find_if(begin(m_enemies), end(m_enemies),
                        [rem_id](const efwk::enemy& e) {
                                return e.id == rem_id;
                        });

                if (found_enemy != end(m_enemies)) {
                        *found_enemy = std::move(m_enemies.back());
                        m_enemies.pop_back();
                        return;
                }
        });

        // Handle creations messages.

        m_cbus.bullet_reqs.visit(dt, [this](efwk::bullet& b) {
                b.id = next_id(); // Fix the id.
                m_bullets.push_back(b);
        });

        spawn_enemy_process(dt);
}

void game::draw(double weight)
{
        al_clear_to_color(al_map_rgb_f(0, 0, 0));

        efwk::display_ent(m_player, weight);

        for (auto& b : m_bullets) {
                efwk::display_ent(b, weight);
        }

        for (auto& e : m_enemies) {
                efwk::display_ent(e, weight);
        }
}

}
