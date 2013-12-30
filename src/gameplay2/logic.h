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

#ifndef LOGIC_H
#define LOGIC_H

#include "../efwk2/sys/bounding.h"
#include "../efwk2/sys/display.h"
#include "../efwk2/sys/movement.h"
#include "../efwk2/sys/input.h"
#include "../efwk2/sys/collision.h"
#include "../efwk2/sys/pain.h"
#include "../efwk2/sys/hud.h"

namespace gplay
{

class pre_collision_update_func
{
        const std::map<int, bool>& m_keys;
        const res::resman& m_resman;
        efwk::comm_bus& m_cbus;

        double m_dt;

public:
        pre_collision_update_func(const std::map<int, bool>& keys,
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
                efwk::weapon_input(ent, m_keys, m_dt, m_cbus);
                efwk::move(ent, m_dt);
                efwk::bind_movement(ent);
                efwk::bind_life(ent, m_cbus);
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

struct collide_func
{
        template <class First, class Second>
        void operator()(First& first, Second& second)
        {
                efwk::check_collisions(first, second);
        }
};

class post_collision_update_func
{
        efwk::comm_bus& m_cbus;

public:
        post_collision_update_func(efwk::comm_bus& cbus) : m_cbus(cbus) {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                efwk::pain(ent, m_cbus);
        }
};

class handle_event_func
{
        gplay::player& m_player;
        int& m_player_score;

public:
        handle_event_func(gplay::player& player,
                          int& player_score) :
                m_player(player),
                m_player_score(player_score)
        {
        }

        void operator()(const efwk::death_event& ev)
        {
                if (ev.score_id == m_player.id) {
                        m_player_score += 10;
                }
        }
};

class try_remove_func
{
        long m_id;

public:
        try_remove_func(long id) : m_id(id) {}

        template <class Entity>
        bool operator()(std::vector<Entity>& col)
        {
                auto found = std::find_if(
                        begin(col), end(col),
                        [this](const Entity& ent)
                        {
                                return ent.id == m_id;
                        });

                if (found == end(col))
                        return false;

                *found = std::move(col.back());
                col.pop_back();
                return true;
        }
};

class draw_func
{
        bool m_debug_key;
        double m_weight;
        ALLEGRO_FONT* m_debug_font;

public:
        draw_func(bool debug_key, double weight, ALLEGRO_FONT* debug_font) :
                m_debug_key(debug_key),
                m_weight(weight),
                m_debug_font(debug_font)
        {}

        template <class Entity>
        void operator()(const Entity& ent)
        {
                efwk::display(ent, m_weight);
                if (m_debug_key)
                        efwk::display_dbg(ent, m_weight, m_debug_font);
        }
};

}

#endif
