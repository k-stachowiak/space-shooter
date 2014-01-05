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

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "../resources/resman.h"

#include "ent_projectile.h"
#include "ent_enemy.h"
#include "ent_player.h"
#include "ent_spark.h"
#include "ent_anim_sprite.h"

#include "../misc/logger.h"

namespace gplay
{

class entity_constructor
{
        const res::resman& m_resman;

        anim_sprite m_make_smoke(long id,
                                 ALLEGRO_BITMAP* bmp,
                                 int frame_width,
                                 double x,
                                 double y) const
        {
                // TODO: enable in the script multiplication by not only literal.
                static int num_frames = cfg::integer("gfx_smoke_num_frames");
                static double frame_time = cfg::real("gfx_smoke_frame_time");

                std::vector<std::pair<int, double>> frame_defs;
                for (int i = 0; i < num_frames; ++i)
                        frame_defs.emplace_back(i, frame_time);

                return {
                        id,
                        num_frames * frame_time,
                        bmp,
                        frame_width,
                        num_frames,
                        frame_defs,
                        -1,
                        x, y
                };
        }

public:
        entity_constructor(const res::resman& rman) :
                m_resman(rman)
        {}

        player make_player(long id, const std::map<int, bool>& keys, double x, double y) const
        {
                logger::instance().trace("Initializing player.");
                return {
                        id,
                        m_resman.get_bitmap(res::res_id::PLAYER_SHIP),
                        m_resman.get_bitmap(res::res_id::PLAYER_SHIP_FLASH),
                        400.0,
                        keys,
                        x, y, -3.1415 / 2.0,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        0.1, 1.0,
                        24.0,
                        100.0,
                        100.0,
                        3
                };
        }

        anim_sprite make_explosion(long id, double x, double y) const
        {
                static int frame_width = cfg::integer("gfx_explosion_frame_width");
                static int num_frames = cfg::integer("gfx_explosion_num_frames");
                static double frame_time = cfg::real("gfx_explosion_frame_time");

                std::vector<std::pair<int, double>> frame_defs;
                for (int i = 0; i < num_frames; ++i)
                        frame_defs.emplace_back(i, frame_time);

                return {
                        id,
                        num_frames * frame_time,
                        m_resman.get_bitmap(res::res_id::EXPLOSION),
                        frame_width,
                        num_frames,
                        frame_defs,
                        -1,
                        x, y
                };
        }

        anim_sprite make_smoke_small(long id, double x, double y) const
        {
                static ALLEGRO_BITMAP* bmp = m_resman.get_bitmap(res::res_id::SMOKE_SMALL);
                static int frame_width = cfg::real("gfx_smoke_frame_width_sml");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        anim_sprite make_smoke_medium(long id, double x, double y) const
        {
                static ALLEGRO_BITMAP* bmp = m_resman.get_bitmap(res::res_id::SMOKE);
                static int frame_width = cfg::real("gfx_smoke_frame_width_med");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        anim_sprite make_smoke_big(long id, double x, double y) const
        {
                static ALLEGRO_BITMAP* bmp = m_resman.get_bitmap(res::res_id::SMOKE_BIG);
                static int frame_width = cfg::real("gfx_smoke_frame_width_big");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        projectile make_bullet(long id, long score_id,
                               double x, double y,
                               bool is_enemy,
                               double damage) const
        {
                const double vx = 0.0;
                const double vy = is_enemy ? 1.0 : -1;
                const double phi = is_enemy ? (3.1415 / 2.0) : (-3.1415 / 2.0);

                static ALLEGRO_BITMAP* bmp = m_resman.get_bitmap(res::res_id::BULLET_5);

                return {
                        id,
                        score_id,
                        bmp,
                        800.0, vx, vy,
                        0,
                        x, y, phi,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        5.0, is_enemy, damage,
                        0.0,
                        0.0,
                        3, 0
                };
        }

        projectile make_missile(long id, long score_id,
                                double x, double y,
                                bool is_enemy,
                                double damage) const
        {
                const double vx = 0.0;
                const double vy = is_enemy ? 1.0 : -1;
                const double phi = is_enemy ? (3.1415 / 2.0) : (-3.1415 / 2.0);

                static ALLEGRO_BITMAP* bmp = m_resman.get_bitmap(res::res_id::MISSILE);

                return {
                        id,
                        score_id,
                        bmp,
                        500.0, vx, vy,
                        1000,
                        x, y, phi,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        5.0, is_enemy, damage,
                        0.01,
                        0.05,
                        10, 1
                };
        }

        spark make_spark(long id,
                         double ttl,
                         std::array<double, 3> rgb,
                         double x, double y,
                         double vx, double vy) const
        {
                return { id, ttl, rgb, vx, vy, x, y };
        }

        enemy make_light_fighter(long id, double x, double y) const
        {
                logger::instance().trace("Initializing light fighter.");
                return {
                        id,
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER),
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER_FLASH),
                        100.0,
                        x, y,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        40.0,
                        100.0,
                        50.0,
                        1
                };
        }

};

}

#endif
