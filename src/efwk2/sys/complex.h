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

#ifndef COMPLEX_H
#define COMPLEX_H

#include "bounding.h"
#include "display.h"
#include "movement.h"
#include "input.h"
#include "collision.h"
#include "pain.h"
#include "hud.h"
#include "emissions.h"

namespace efwk
{

class pre_collision_update_func
{
        const std::map<int, bool>& m_keys;
        const res::resman& m_resman;
        comm_bus& m_cbus;

        double m_dt;

public:
        pre_collision_update_func(const std::map<int, bool>& keys,
                                  const res::resman& resman,
                                  comm_bus& cbus,
                                  double dt) :
                m_keys(keys),
                m_resman(resman),
                m_cbus(cbus),
                m_dt(dt)
        {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                weapon_input(ent, m_keys, m_dt, m_cbus);
                move(ent, m_dt);
                bind_movement(ent);
                bind_life(ent, m_cbus);
                bind_time(ent, m_dt, m_cbus);
                display_update(ent, m_dt);
                emit(ent, m_dt, m_cbus);
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
                check_collisions(first, second);
        }
};

class post_collision_update_func
{
        comm_bus& m_cbus;

public:
        post_collision_update_func(comm_bus& cbus) : m_cbus(cbus) {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                pain(ent, m_cbus);
        }
};

class handle_event_func
{
        long m_player_id;
        int& m_player_score;

public:
        handle_event_func(long player_id,
                          int& player_score) :
                m_player_id(player_id),
                m_player_score(player_score)
        {
        }

        void operator()(const death_event& ev)
        {
                if (ev.score_id == m_player_id) {
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
                display(ent, m_weight);
                if (m_debug_key)
                        display_dbg(ent, m_weight, m_debug_font);
        }
};

}

#endif
