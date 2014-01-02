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

#include "ent_bullet.h"
#include "ent_enemy.h"
#include "ent_player.h"
#include "ent_spark.h"
#include "ent_anim_sprite.h"

namespace gplay
{

inline
player make_player(long id,
                   ALLEGRO_BITMAP* bmp,
                   const std::map<int, bool>& keys,
                   double x, double y)
{
        return {
                id, bmp,
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

inline
anim_sprite make_explosion(long id, ALLEGRO_BITMAP* bmp, double x, double y)
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
                bmp,
                frame_width,
                num_frames,
                frame_defs,
                -1,
                x, y
        };
}

inline
bullet make_bullet(long id, long score_id,
                   double x, double y,
                   bool is_enemy,
                   double damage,
                   ALLEGRO_BITMAP* bmp)
{
        const double vx = 0.0;
        const double vy = is_enemy ? 1.0 : -1;
        const double phi = is_enemy ? (3.1415 / 2.0) : (-3.1415 / 2.0);

        return {
                id, score_id, bmp,
                800.0, vx, vy, x, y, phi,
                0, 0,
                static_cast<double>(cfg::integer("gfx_screen_w")),
                static_cast<double>(cfg::integer("gfx_screen_h")),
                5.0, is_enemy, damage
        };
}

inline
spark make_spark(long id,
                 double ttl,
                 std::array<double, 3> rgb,
                 double x, double y,
                 double vx, double vy)
{
        return { id, ttl, rgb, vx, vy, x, y };
}

inline
enemy make_light_fighter(long id,
                         ALLEGRO_BITMAP* bmp,
                         double x, double y)
{
        return {
                id, bmp,
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

}

#endif
