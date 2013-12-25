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

#include <functional>
#include <sstream>

#include "game.h"

#include "../misc/logger.h"
#include "../misc/config.h"

namespace
{

// Generic operations on the whole entities.
// -----------------------------------------

class update_func
{
        const std::map<int, bool>& m_keys;
        const res::resman& m_resman;
        efwk::comm_bus& m_cbus;

        double m_dt;

public:
        update_func(const std::map<int, bool>& keys,
                    const res::resman& resman,
                    efwk::comm_bus& cbus,
                    double dt) :
                m_keys(keys),
                m_resman(resman),
                m_cbus(cbus),
                m_dt(dt)
        {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                efwk::weapon_input(ent, m_keys, m_dt, m_resman, m_cbus);
                efwk::move(ent, m_dt);
                efwk::bind_movement(ent);
                efwk::bind_life(ent, m_cbus);
        }
};

class draw_func
{
        double m_weight;

public:
        draw_func(double weight) : m_weight(weight) {}

        template <class Entity>
        void operator()(const Entity& ent)
        {
                efwk::display(ent, m_weight);
                efwk::display_dbg(ent, m_weight);
        }
};

struct collq_clear_func
{
        template <class Entity>
        void operator()(Entity& ent)
        {
                ent.collq.clear();
        }
};

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

}

namespace gplay
{

void game::log_state()
{
        std::stringstream ss;

        ss << "Frame logic state:" << std::endl;
        ss << "Bullets count: " << m_bullets.size() << "." << std::endl;
        ss << "Enemies count: " << m_enemies.size() << ".";

        logger::instance().trace(ss.str());
}

void game::update_entities(double dt)
{
        update_func uf { m_keys, m_resman, m_cbus, dt };
        efwk::map(uf, m_player, m_bullets, m_enemies);
}

void game::handle_collisions()
{
        efwk::map(collq_clear_func(), m_player, m_bullets, m_enemies);
        efwk::collide_all(m_player, m_bullets, m_enemies);
}

void game::handle_deletions(double dt)
{
        m_cbus.dels.visit(dt, [this](long rem_id) {
                if (try_remove_ent(m_bullets, rem_id))
                        return;
                if (try_remove_ent(m_enemies, rem_id))
                        return;
                if (rem_id == m_player.id)
                        throw; // TODO: enable state end here.
        });
}

void game::handle_creations(double dt)
{
        // Handle creation messages.
        m_cbus.bullet_reqs.visit(dt, [this](efwk::bullet_req& brq) {
                m_bullets.emplace_back(
                        next_id(),
                        m_resman.get_bitmap(res::res_id::BULLET_5),
                        800.0, brq.vx, brq.vy,
                        brq.x, brq.y, -3.1415 / 2.0,
                        0, 0,
                        cfg::integer("gfx_screen_w"),
                        cfg::integer("gfx_screen_h"),
                        5.0);
        });

        // Spawn enemy.
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
                cfg::integer("gfx_screen_h"),
                32.0);
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
                0.1, 1.0,
                24.0)
{
}

void game::update(double dt)
{
        log_state();
        update_entities(dt);
        handle_collisions();
        handle_deletions(dt);
        handle_creations(dt);
}

void game::draw(double weight)
{
        al_clear_to_color(al_map_rgb_f(0, 0, 0));
        draw_func df { weight };
        efwk::map(df, m_player, m_bullets, m_enemies);
}

}
