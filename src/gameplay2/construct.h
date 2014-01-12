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
#include "ent_pickup.h"
#include "ent_anim_sprite.h"

#include "../misc/logger.h"

namespace gplay
{

class entity_constructor
{
        const res::resman& m_resman;

        anim_sprite m_make_smoke(const long id,
                                 ALLEGRO_BITMAP* const bmp,
                                 const int frame_width,
                                 const double x,
                                 const double y) const
        {
                // TODO: enable in the script multiplication by not only literal.
                static const int num_frames = cfg::integer("gfx_smoke_num_frames");
                static const double frame_time = cfg::real("gfx_smoke_frame_time");

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

        player make_player(const long id,
                           const std::map<int, bool>& keys,
                           const double x,
                           const double y) const
        {
                return {
                        id,
                        m_resman.get_bitmap(res::res_id::PLAYER_SHIP),
                        m_resman.get_bitmap(res::res_id::PLAYER_SHIP_FLASH),
                        cfg::real("gameplay_player_vel"),
                        keys,
                        x, y, -3.1415 / 2.0,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        cfg::real("gameplay_player_gun_interval"),
                        cfg::real("gameplay_player_rl_interval"),
                        cfg::real("gameplay_player_shape_radius"),
                        cfg::real("gameplay_player_ship_damage"),
                        cfg::real("gameplay_player_max_health")
                };
        }

        anim_sprite make_explosion(const long id,
                                   const double x,
                                   const double y) const
        {
                static const int frame_width = cfg::integer("gfx_explosion_frame_width");
                static const int num_frames = cfg::integer("gfx_explosion_num_frames");
                static const double frame_time = cfg::real("gfx_explosion_frame_time");

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

        anim_sprite make_smoke_small(const long id, const double x, const double y) const
        {
                static ALLEGRO_BITMAP* const bmp = m_resman.get_bitmap(res::res_id::SMOKE_SMALL);
                static const int frame_width = cfg::real("gfx_smoke_frame_width_sml");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        anim_sprite make_smoke_medium(const long id, const double x, const double y) const
        {
                static ALLEGRO_BITMAP* const bmp = m_resman.get_bitmap(res::res_id::SMOKE);
                static const int frame_width = cfg::real("gfx_smoke_frame_width_med");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        anim_sprite make_smoke_big(const long id, const double x, const double y) const
        {
                static ALLEGRO_BITMAP* const bmp = m_resman.get_bitmap(res::res_id::SMOKE_BIG);
                static const int frame_width = cfg::real("gfx_smoke_frame_width_big");
                return m_make_smoke(id, bmp, frame_width, x, y);
        }

        projectile make_bullet(const long id,
                               const long score_id,
                               const double x,
                               const double y,
                               const bool is_enemy,
                               const double damage) const
        {
                const double vx = 0.0;
                const double vy = is_enemy ? 1.0 : -1;
                const double phi = is_enemy ? (3.1415 / 2.0) : (-3.1415 / 2.0);

                static ALLEGRO_BITMAP* const bmp = m_resman.get_bitmap(res::res_id::BULLET_5);

                return {
                        id,
                        score_id,
                        bmp,
                        cfg::real("gameplay_bullet_lin_vel"),
                        vx, vy,
                        0,
                        x, y, phi,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        cfg::real("gameplay_bullet_shape_radius"),
                        is_enemy,
                        damage,
                        0.0,
                        0.0,
                        cfg::integer("gameplay_bullet_sparks_on_death"),
                        0
                };
        }

        projectile make_missile(const long id,
                                const long score_id,
                                const double x,
                                const double y,
                                const bool is_enemy,
                                const double damage) const
        {
                const double vx = 0.0;
                const double vy = is_enemy ? 1.0 : -1;
                const double phi = is_enemy ? (3.1415 / 2.0) : (-3.1415 / 2.0);

                static ALLEGRO_BITMAP* const bmp = m_resman.get_bitmap(res::res_id::MISSILE);

                return {
                        id,
                        score_id,
                        bmp,
                        cfg::real("gameplay_missile_lin_vel"),
                        vx, vy,
                        cfg::real("gameplay_missile_lin_acc"),
                        x, y, phi,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        cfg::real("gameplay_missile_shape_radius"),
                        is_enemy,
                        damage,
                        cfg::real("gameplay_missile_spark_interval"),
                        cfg::real("gameplay_missile_smoke_interval"),
                        cfg::integer("gameplay_missile_sparks_on_death"),
                        1
                };
        }

        spark make_spark(const long id,
                         const double ttl,
                         const std::array<double, 3> rgb,
                         const double x,
                         const double y,
                         const double vx,
                         const double vy) const
        {
                return { id, ttl, rgb, vx, vy, x, y };
        }

        pickup make_health_pickup(const long id,
                                  const long score_id,
                                  const double x,
                                  const double y,
                                  const double vx,
                                  const double vy) const
        {
                return {
                        id,
                        score_id,
                        m_resman.get_bitmap(res::res_id::HEALTH),
                        vx, vy,
                        x, y,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        cfg::real("gameplay_pickup_shape_radius"),
                        cfg::real("gameplay_pickup_health_amount")
                };
        }

        enemy make_light_fighter(const long id, const double x, const double y) const
        {
                return {
                        id,
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER),
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER_FLASH),
                        100.0,
                        x, y,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        cfg::real("gameplay_lfighter_shape_radius"),
                        -1.0, -1.0, -1.0,       // radius 1, 2, 3
                        -1.0, -1.0,             // c1x, c1y,
                        -1.0, -1.0,             // c2x, c2y,
                        -1.0, -1.0,             // c3x, c3y,
                        true,                   // is_single_circle
                        cfg::real("gameplay_lfighter_damage"),
                        cfg::real("gameplay_lfighter_max_health"),
                        cfg::integer("gameplay_lfighter_num_explosions"),
                        cfg::real("gameplay_lfighter_health_drop_dist"),
                        cfg::real("gameplay_lfighter_pain_smoke_threshold"),
                        cfg::real("gameplay_lfighter_pain_smoke_interval")
                };
        }

        enemy make_light_bomber(const long id, const double x, const double y) const
        {
                return {
                        id,
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_BOMBER),
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_BOMBER_FLASH),
                        75,
                        x, y,
                        0, 0,
                        static_cast<double>(cfg::integer("gfx_screen_w")),
                        static_cast<double>(cfg::integer("gfx_screen_h")),
                        -1.0,           // radius 0
                        cfg::real("gameplay_lbomber_shape_radius"),
                        cfg::real("gameplay_lbomber_shape_radius"),
                        cfg::real("gameplay_lbomber_shape_radius"),
                        cfg::real("gameplay_lbomber_shape1_xoffset"),
                        cfg::real("gameplay_lbomber_shape1_yoffset"),
                        cfg::real("gameplay_lbomber_shape2_xoffset"),
                        cfg::real("gameplay_lbomber_shape2_yoffset"),
                        cfg::real("gameplay_lbomber_shape3_xoffset"),
                        cfg::real("gameplay_lbomber_shape3_yoffset"),
                        false,          // is_single_circle
                        cfg::real("gameplay_lbomber_damage"),
                        cfg::real("gameplay_lbomber_max_health"),
                        cfg::integer("gameplay_lbomber_num_explosions"),
                        cfg::real("gameplay_lbomber_health_drop_dist"),
                        cfg::real("gameplay_lbomber_pain_smoke_threshold"),
                        cfg::real("gameplay_lbomber_pain_smoke_interval"),
                };
        }

};

}

#endif
