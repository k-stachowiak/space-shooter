/* Copyright (C) 2012,2013 Krzysztof Stachowiak */

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

#include <iostream>
using std::cout;
using std::endl;

#include "../misc/config.h"
#include "../misc/rand.h"
#include "../geometry/bezier.h"
#include "game_ent_fac.h"

using namespace res;

uint64_t entity_factory::create_explosion(double x, double y) {

        // Helpers.
        // --------

        uint32_t frame_width = cfg::integer("gfx_explosion_frame_width");
        uint32_t num_frames = cfg::integer("gfx_explosion_num_frames");
        double frame_time = cfg::real("gfx_explosion_frame_time");

        vector<cmp::frame_def> frame_defs;
        for(uint32_t i = 0; i < num_frames; ++i)
                frame_defs.emplace_back(i, frame_time);

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0); 
        shared_ptr<cmp::dynamics> dynamics; 
        shared_ptr<cmp::shape> shape; 

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::FX;
        auto appearance = cmp::create_simple_anim(
                        _resman.get_bitmap(res_id::EXPLOSION),
                        frame_width,
                        num_frames,
                        frame_defs,
                        1);

        // Wellness components.
        // --------------------
        auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 
        shared_ptr<cmp::reaction> on_death;
        shared_ptr<cmp::wellness> wellness; 

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Fx components.
        // --------------
        shared_ptr<cmp::fx> fxs;

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _sound_system.add_node({ id, nqueue });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });

        return id;
}

uint64_t entity_factory::create_smoke(double x, double y, comm::smoke_size size) {

        // Helpers.
        // --------

        // Interpret size.
        res_id rid;
        double scale;
        switch(size) {
                case comm::smoke_size::tiny:
                        rid = res_id::SMOKE_SMALL;
                        scale = cfg::real("gfx_smoke_scale_tiny");
                        break;

                case comm::smoke_size::medium:
                        rid = res_id::SMOKE;
                        scale = cfg::real("gfx_smoke_scale_medium");
                        break;
                        
                case comm::smoke_size::big:
                        rid = res_id::SMOKE_BIG;
                        scale = cfg::real("gfx_smoke_scale_big");
                        break;

                default:
                        throw;
        }

        // Prepare animation frame definitions.
        uint32_t frame_width = cfg::real("gfx_smoke_frame_width_base") * scale;
        uint32_t num_frames = cfg::integer("gfx_smoke_num_frames");
        double frame_time = cfg::real("gfx_smoke_frame_time");
        vector<cmp::frame_def> frame_defs;
        for(uint32_t i = 0; i < num_frames; ++i)
                frame_defs.emplace_back(i, frame_time);

        
        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0); 
        shared_ptr<cmp::dynamics> dynamics; 
        shared_ptr<cmp::shape> shape; 

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::FX;
        auto appearance = cmp::create_simple_anim(
                        _resman.get_bitmap(rid),
                        frame_width,
                        num_frames,
                        frame_defs,
                        1);

        // Wellness components.
        // --------------------
        auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 
        shared_ptr<cmp::reaction> on_death;
        shared_ptr<cmp::wellness> wellness; 

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Fx components.
        // --------------
        shared_ptr<cmp::fx> fxs;

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _sound_system.add_node({ id, nqueue });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });

        return id;
}

uint64_t entity_factory::create_debris(double x, double y,
                double bvx, double bvy,
                double vmin, double vmax,
                double theta_min, double theta_max,
                res_id bmp,
                bool explode,
                uint64_t origin_id) {

        // Helpers.
        // --------
        double debris_health = 1;

        // TTL generation.
        uniform_real_distribution<double> ttl_dist(
                        cfg::real("gameplay_debris_ttl_min"),
                        cfg::real("gameplay_debris_ttl_max"));
        double ttl_time = ttl_dist(rnd::engine);

        // Random movement.
        bernoulli_distribution dir_dist;
        
        uniform_real_distribution<double> mv_dist(vmin, vmax);
        double base_vx = mv_dist(rnd::engine);
        double base_vy = mv_dist(rnd::engine);
        double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
        double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

        uniform_real_distribution<double> rot_dist(theta_min, theta_max);
        double base_av = rot_dist(rnd::engine);
        double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0); 
        auto shape = cmp::create_circle(cfg::real("gameplay_debris_shape_radius"));

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::FX;
        auto appearance = cmp::create_static_bmp(_resman.get_bitmap(bmp));

        // Wellness nodes.
        // ---------------
        auto wellness = cmp::create_wellness(debris_health, 0.0);
        auto ttl = cmp::create_const_int_timer(ttl_time); 
        auto on_death = explode
                ? cmp::create_explosion_sequence_reaction(1,
                                cfg::real("gfx_explosion_seq_min_delay"),
                                cfg::real("gfx_explosion_seq_min_delay"))
                : shared_ptr<cmp::reaction>();

        // Movement nodes.
        // ---------------
        auto movement_bounds = shared_ptr<cmp::bounds>(); 
        auto life_bounds = cmp::create_bounds(0.0, 0.0, cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"));
        auto dynamics = cmp::create_complex_dynamics({
                cmp::create_const_velocity_dynamics(bvx + base_vx * mul_vx, bvy + base_vy * mul_vy),
                cmp::create_const_ang_vel_dynamics(base_av * mul_av)
        });

        // Collision nodes.
        // ----------------
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::NONE,
                cmp::coll_class::PROJECTILE,
                true,
                cfg::real("gameplay_debris_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
        _collision_system.add_node({ id, origin_id, orientation, cp, pp, shape, coll_queue, "debris" }); 
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_star() {
        uniform_real_distribution<double> uni_dist;
        double x = uni_dist(rnd::engine) * cfg::integer("gfx_screen_w");
        return create_star_xy(x, 1.0);
}

uint64_t entity_factory::create_star_xy(double x, double y) {

        // Helpers.
        // --------
        uniform_real_distribution<double> uni_dist;
        double grade = uni_dist(rnd::engine);
        double vy = cfg::real("gameplay_star_vel_base")
                        * (grade * cfg::real("gameplay_star_vel_grade_scale") +
                        +  cfg::real("gameplay_star_vel_grade_offset"));
        double shade = grade
                        * cfg::real("gameplay_star_shade_grade_scale")
                        + cfg::real("gameplay_star_shade_grade_offset");
        
        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0);
        shared_ptr<cmp::shape> shape;

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::BACKGROUND;
        auto appearance = cmp::create_pixel(shade, shade, shade);

        // Movement components.
        // --------------------
        auto dynamics = cmp::create_const_velocity_dynamics(0.0, vy);
        auto movement_bounds = shared_ptr<cmp::bounds>(); 
        auto life_bounds = cmp::create_bounds(0.0, 0.0, cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"));

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });

        return id;
}

uint64_t entity_factory::create_player_ship(double x, double y) {

        // Initialize components.
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, -cfg::math::half_pi);
        auto shape = cmp::create_circle(cfg::real("gameplay_player_shape_radius"));
        auto flash_flag = make_shared<bool>(false);

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::SHIPS;
        auto appearance_base = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::PLAYER_SHIP));
        auto appearance_flash = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::PLAYER_SHIP_FLASH));

        auto appearance = cmp::create_bin_proxy_appr(
                flash_flag,
                appearance_flash,
                appearance_base);

        // Movement components.
        // --------------------
        auto life_bounds = shared_ptr<cmp::bounds>();
        auto dynamics = cmp::create_player_controlled_dynamics();
        auto movement_bounds = cmp::create_bounds(
                        0.0, 0.0,
                        cfg::integer("gfx_screen_w"),
                        cfg::integer("gfx_screen_h")
                                - cfg::real("gameplay_player_bottom_bound_offset"));

        // Arms components.
        // ----------------
        auto weapon_beh = cmp::create_player_controlled_weapon_beh();
        auto upgrades = cmp::create_upgrades(
                        cfg::integer("gameplay_player_max_gun_upgrades"),
                        cfg::integer("gameplay_player_max_rl_upgrades"),
                        cfg::integer("gameplay_player_max_gun_upgrade_ammo"),
                        cfg::integer("gameplay_player_max_rl_upgrade_ammo"));

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue(); 
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::PLAYER,
                cmp::coll_class::SHIP,
                false,
                cfg::real("gameplay_player_ship_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(
                        cfg::real("gameplay_player_max_health"),
                        cfg::real("gameplay_player_max_shield"));
        auto on_death = cmp::create_complex_reaction({
                cmp::create_debris_reaction(
                        cfg::integer("gameplay_player_num_debris"),
                        {
                                res_id::DEBRIS1,
                                res_id::DEBRIS2,
                                res_id::DEBRIS3,
                                res_id::DEBRIS4,
                                res_id::DEBRIS5
                        },
                        cfg::real("gameplay_debris_min_vel"),
                        cfg::real("gameplay_debris_max_vel"),
                        cfg::real("gameplay_debris_min_theta"),
                        cfg::real("gameplay_debris_max_theta"),
                        /* explode = */ false,
                        /* randomize = */ true),
                cmp::create_explosion_sequence_reaction(
                                cfg::integer("gameplay_player_num_explosions"),
                                cfg::real("gfx_explosion_seq_min_delay"),
                                cfg::real("gfx_explosion_seq_min_delay"))
        });

        shared_ptr<cmp::timer> ttl; 

        // Fx components.
        // --------------
        auto fxs = cmp::create_complex_fx({
            cmp::create_smoke_when_hurt(
                cmp::create_const_int_timer(cfg::real("gameplay_smoke_interval")),
                cfg::real("gameplay_smoke_health_ratio_threshold")),
            cmp::create_flash_when_hurt(flash_flag, cfg::real("gfx_pain_flash_timer"))
        });

        // Score components.
        // -----------------
        auto score = make_shared<double>();
        auto sc = cmp::score_class::PLAYER;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
        _arms_system.add_node({ id, orientation, weapon_beh, upgrades, nqueue });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "player" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _pickup_system.add_node({ id, coll_queue, wellness, upgrades, nqueue });
        _input_system.add_node({ id, dynamics, weapon_beh });
        _score_system.add_node({ id, sc, score, wellness });
        _hud_system.add_node({ id, score, wellness, upgrades });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_light_fighter_dyn(double x, double y, shared_ptr<cmp::dynamics> dynamics) {

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, cfg::math::half_pi);
        auto shape = cmp::create_circle(cfg::real("gameplay_lfighter_shape_radius"));
        auto flash_flag = make_shared<bool>(false);

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::SHIPS;
        auto appearance_base = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER)); 
        auto appearance_flash = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER_FLASH)); 

        auto appearance = cmp::create_bin_proxy_appr(
                flash_flag,
                appearance_flash,
                appearance_base);

        // Movement components.
        // --------------------
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                cfg::integer("gfx_screen_w") + cfg::real("gameplay_wide_offscreen_bound") + 1.0,
                cfg::integer("gfx_screen_h") + cfg::real("gameplay_wide_offscreen_bound") + 1.0);

        // Arms components.
        // ----------------
        auto upgrades = cmp::create_upgrades(0, 0, 0, 0);
        auto weapon_beh = cmp::create_complex_weapon_beh({
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_lfighter_bullet_interval"),
                                        cfg::real("gameplay_lfighter_bullet_interval"),
                                        +cfg::real("gfx_lfighter_gun_xoffset"),
                                        cfg::real("gfx_lfighter_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),

                        cmp::create_period_bullet(
                                        cfg::real("gameplay_lfighter_bullet_interval"),
                                        cfg::real("gameplay_lfighter_bullet_interval"),
                                        -cfg::real("gfx_lfighter_gun_xoffset"),
                                        cfg::real("gfx_lfighter_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")) });

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::ENEMY,
                cmp::coll_class::SHIP,
                false,
                cfg::real("gameplay_lfighter_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(
                        cfg::real("gameplay_lfighter_max_health"),
                        cfg::real("gameplay_lfighter_max_shield"));
        vector<shared_ptr<cmp::reaction>> reactions {
                cmp::create_debris_reaction(
                                cfg::integer("gameplay_lfighter_num_debris"),
                                {
                                        res_id::DEBRIS1,
                                        res_id::DEBRIS2,
                                        res_id::DEBRIS3,
                                        res_id::DEBRIS4,
                                        res_id::DEBRIS5
                                },
                                cfg::real("gameplay_debris_min_vel"),
                                cfg::real("gameplay_debris_max_vel"),
                                cfg::real("gameplay_debris_min_theta"),
                                cfg::real("gameplay_debris_max_theta"),
                                /* explode = */ false,
                                /* randomize = */ true),
                                cmp::create_explosion_sequence_reaction(
                                                cfg::integer("gameplay_lfighter_num_explosions"),
                                                cfg::real("gfx_explosion_seq_min_delay"),
                                                cfg::real("gfx_explosion_seq_min_delay")) };

        bernoulli_distribution drop_health(cfg::real("gameplay_lfighter_health_drop_dist"));
        if(drop_health(rnd::engine))
                reactions.push_back(cmp::create_health_drop_reaction());

        bernoulli_distribution drop_battery(cfg::real("gameplay_lfighter_battery_drop_dist"));
        if(drop_battery(rnd::engine))
                reactions.push_back(cmp::create_battery_drop_reaction());

        bernoulli_distribution drop_bul_up(cfg::real("gameplay_lfighter_gun_up_drop_dist"));
        if(drop_bul_up(rnd::engine))
                reactions.push_back(cmp::create_bullet_upgrade_drop_reaction());

        bernoulli_distribution drop_mis_up(cfg::real("gameplay_lfighter_ml_up_drop_dist"));
        if(drop_mis_up(rnd::engine))
                reactions.push_back(cmp::create_missile_upgrade_drop_reaction());

        auto on_death = cmp::create_complex_reaction(reactions);
        shared_ptr<cmp::timer> ttl;

        // Fx components.
        // --------------
        auto fxs = cmp::create_complex_fx({
            cmp::create_smoke_when_hurt(
                cmp::create_const_int_timer(cfg::real("gameplay_smoke_interval")),
                cfg::real("gameplay_smoke_health_ratio_threshold")),
            cmp::create_flash_when_hurt(flash_flag, cfg::real("gfx_pain_flash_timer"))
        });

        // Score components.
        // -----------------
        auto score = make_shared<double>();
        auto sc = cmp::score_class::ENEMY_LIGHT_FIGHTER;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register components.
        // --------------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
        _arms_system.add_node({ id, orientation, weapon_beh, upgrades, nqueue });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "l_fighter" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _score_system.add_node({ id, sc, score, wellness });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_heavy_fighter_dyn(double x, double y, shared_ptr<cmp::dynamics> dynamics) {

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, cfg::math::half_pi);
        auto shape = cmp::create_circle(cfg::real("gameplay_hfighter_shape_radius"));
        auto flash_flag = make_shared<bool>(false);

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::SHIPS;
        auto appearance_base = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER)); 
        auto appearance_flash = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER_FLASH)); 

        auto appearance = cmp::create_bin_proxy_appr(
                flash_flag,
                appearance_flash,
                appearance_base);

        // Movement components.
        // --------------------
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                cfg::integer("gfx_screen_w") + cfg::real("gameplay_wide_offscreen_bound") + 1.0,
                cfg::integer("gfx_screen_h") + cfg::real("gameplay_wide_offscreen_bound") + 1.0);

        // Arms components.
        // ----------------
        auto upgrades = cmp::create_upgrades(0, 0, 0, 0);
        auto weapon_beh = cmp::create_complex_weapon_beh({
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_hfighter_bullet_interval"),
                                        cfg::real("gameplay_hfighter_bullet_interval"),
                                        +cfg::real("gfx_hfighter_gun_xoffset"),
                                        cfg::real("gfx_hfighter_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_hfighter_bullet_interval"),
                                        cfg::real("gameplay_hfighter_bullet_interval"),
                                        -cfg::real("gfx_hfighter_gun_xoffset"),
                                        cfg::real("gfx_hfighter_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_hfighter_missile_interval"),
                                        cfg::real("gameplay_hfighter_missile_interval"),
                                        +cfg::real("gfx_hfighter_rl_xoffset"),
                                        cfg::real("gfx_hfighter_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_hfighter_missile_interval"),
                                        cfg::real("gameplay_hfighter_missile_interval"),
                                        -cfg::real("gfx_hfighter_rl_xoffset"),
                                        cfg::real("gfx_hfighter_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")) });

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::ENEMY,
                cmp::coll_class::SHIP,
                false,
                cfg::real("gameplay_hfighter_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(
                        cfg::real("gameplay_hfighter_max_health"),
                        cfg::real("gameplay_hfighter_max_shield"));
        shared_ptr<cmp::timer> ttl;
        vector<shared_ptr<cmp::reaction>> reactions {
                cmp::create_debris_reaction(
                                cfg::integer("gameplay_hfighter_num_debris"),
                                {
                                        res_id::DEBRIS1,
                                        res_id::DEBRIS2,
                                        res_id::DEBRIS3,
                                        res_id::DEBRIS4,
                                        res_id::DEBRIS5
                                },
                                cfg::real("gameplay_debris_min_vel"),
                                cfg::real("gameplay_debris_max_vel"),
                                cfg::real("gameplay_debris_min_theta"),
                                cfg::real("gameplay_debris_max_theta"),
                        /* explode = */ false,
                        /* randomize = */ true),
                cmp::create_explosion_sequence_reaction(
                                cfg::integer("gameplay_hfighter_num_explosions"),
                                cfg::real("gfx_explosion_seq_min_delay"),
                                cfg::real("gfx_explosion_seq_min_delay")) };

        bernoulli_distribution drop_health(cfg::real("gameplay_hfighter_health_drop_dist"));
        if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

        bernoulli_distribution drop_battery(cfg::real("gameplay_hfighter_battery_drop_dist"));
        if(drop_battery(rnd::engine))
                reactions.push_back(cmp::create_battery_drop_reaction());

        bernoulli_distribution drop_bul_up(cfg::real("gameplay_hfighter_gun_up_drop_dist"));
        if(drop_bul_up(rnd::engine))
                reactions.push_back(cmp::create_bullet_upgrade_drop_reaction());

        bernoulli_distribution drop_mis_up(cfg::real("gameplay_hfighter_ml_up_drop_dist"));
        if(drop_mis_up(rnd::engine))
                reactions.push_back(cmp::create_missile_upgrade_drop_reaction());

        auto on_death = cmp::create_complex_reaction(reactions);

        // Fx components.
        // --------------
        auto fxs = cmp::create_complex_fx({
            cmp::create_smoke_when_hurt(
                cmp::create_const_int_timer(cfg::real("gameplay_smoke_interval")),
                cfg::real("gameplay_smoke_health_ratio_threshold")),
            cmp::create_flash_when_hurt(flash_flag, cfg::real("gfx_pain_flash_timer"))
        });

        // Score components.
        // -----------------
        auto score = make_shared<double>();
        auto sc = cmp::score_class::ENEMY_HEAVY_FIGHTER;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register the components.
        // ------------------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
        _arms_system.add_node({ id, orientation, weapon_beh, upgrades, nqueue });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "h_fighter" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _score_system.add_node({ id, sc, score, wellness });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_light_bomber_dyn(double x, double y, shared_ptr<cmp::dynamics> dynamics) {

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, cfg::math::half_pi);
        auto shape = cmp::create_complex_shape({
                { 
                        cmp::create_circle(cfg::real("gameplay_lbomber_shape_radius")),
                        {
                                cfg::real("gameplay_lbomber_shape1_xoffset"),
                                cfg::real("gameplay_lbomber_shape1_yoffset")
                        }
                },
                {
                        cmp::create_circle(cfg::real("gameplay_lbomber_shape_radius")),
                        {
                                cfg::real("gameplay_lbomber_shape2_xoffset"),
                                cfg::real("gameplay_lbomber_shape2_yoffset")
                        }
                },
                {
                        cmp::create_circle(cfg::real("gameplay_lbomber_shape_radius")),
                        {
                                cfg::real("gameplay_lbomber_shape3_xoffset"),
                                cfg::real("gameplay_lbomber_shape3_yoffset")
                        }
                } 
        });
        auto flash_flag = make_shared<bool>(false);

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::SHIPS;
        auto appearance_base = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER)); 
        auto appearance_flash = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER_FLASH)); 

        auto appearance = cmp::create_bin_proxy_appr(
                flash_flag,
                appearance_flash,
                appearance_base);

        // Movement components.
        // --------------------
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                cfg::integer("gfx_screen_w") + cfg::real("gameplay_wide_offscreen_bound") + 1.0,
                cfg::integer("gfx_screen_h") + cfg::real("gameplay_wide_offscreen_bound") + 1.0);

        // Arms components.
        // ----------------
        auto upgrades = cmp::create_upgrades(0, 0, 0, 0);
        auto weapon_beh = cmp::create_complex_weapon_beh({
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_lbomber_bullet_interval"),
                                        cfg::real("gameplay_lbomber_bullet_interval"),
                                        +cfg::real("gfx_lbomber_gun_xoffset"),
                                        cfg::real("gfx_lbomber_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_lbomber_bullet_interval"),
                                        cfg::real("gameplay_lbomber_bullet_interval"),
                                        -cfg::real("gfx_lbomber_gun_xoffset"),
                                        cfg::real("gfx_lbomber_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_lbomber_missile_interval"),
                                        cfg::real("gameplay_lbomber_missile_interval"),
                                        +cfg::real("gfx_lbomber_rl_xoffset"),
                                        cfg::real("gfx_lbomber_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_lbomber_missile_interval"),
                                        cfg::real("gameplay_lbomber_missile_interval"),
                                        -cfg::real("gfx_lbomber_rl_xoffset"),
                                        cfg::real("gfx_lbomber_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")) });

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::ENEMY,
                cmp::coll_class::SHIP,
                false,
                cfg::real("gameplay_lbomber_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(
                        cfg::real("gameplay_lbomber_max_health"),
                        cfg::real("gameplay_lbomber_max_shield"));
        vector<shared_ptr<cmp::reaction>> reactions {
                cmp::create_debris_reaction(
                                cfg::integer("gameplay_lbomber_num_debris"),
                                {
                                        res_id::DEBRIS1,
                                        res_id::DEBRIS2,
                                        res_id::DEBRIS3,
                                        res_id::DEBRIS4,
                                        res_id::DEBRIS5
                                },
                                cfg::real("gameplay_debris_min_vel"),
                                cfg::real("gameplay_debris_max_vel"),
                                cfg::real("gameplay_debris_min_theta"),
                                cfg::real("gameplay_debris_max_theta"),
                        /* explode = */ false,
                        /* randomize = */ true),
                cmp::create_explosion_sequence_reaction(
                                cfg::integer("gameplay_lbomber_num_explosions"),
                                cfg::real("gfx_explosion_seq_min_delay"),
                                cfg::real("gfx_explosion_seq_min_delay"))
        };

        bernoulli_distribution drop_health(cfg::real("gameplay_lbomber_health_drop_dist"));
        if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

        bernoulli_distribution drop_battery(cfg::real("gameplay_lbomber_battery_drop_dist"));
        if(drop_battery(rnd::engine))
                reactions.push_back(cmp::create_battery_drop_reaction());

        bernoulli_distribution drop_bul_up(cfg::real("gameplay_lbomber_gun_up_drop_dist"));
        if(drop_bul_up(rnd::engine))
                reactions.push_back(cmp::create_bullet_upgrade_drop_reaction());

        bernoulli_distribution drop_mis_up(cfg::real("gameplay_lbomber_ml_up_drop_dist"));
        if(drop_mis_up(rnd::engine))
                reactions.push_back(cmp::create_missile_upgrade_drop_reaction());

        auto on_death = cmp::create_complex_reaction(reactions);
        shared_ptr<cmp::timer> ttl;

        // Fx components.
        // --------------
        auto fxs = cmp::create_complex_fx({
            cmp::create_smoke_when_hurt(
                cmp::create_const_int_timer(cfg::real("gameplay_smoke_interval")),
                cfg::real("gameplay_smoke_health_ratio_threshold")),
            cmp::create_flash_when_hurt(flash_flag, cfg::real("gfx_pain_flash_timer"))
        });

        // Score components.
        // -----------------
        auto score = make_shared<double>();
        auto sc = cmp::score_class::ENEMY_LIGHT_BOMBER;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register the components.
        // ------------------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
        _arms_system.add_node({ id, orientation, weapon_beh, upgrades, nqueue });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "l_bomber" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _score_system.add_node({ id, sc, score, wellness });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_heavy_bomber_dyn(double x, double y, shared_ptr<cmp::dynamics> dynamics) {

        // Common components.
        // -----------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0);
        auto shape = cmp::create_complex_shape({
                {
                        cmp::create_circle(cfg::real("gameplay_hbomber_shape_radius")),
                        {
                                cfg::real("gameplay_hbomber_shape1_xoffset"),
                                cfg::real("gameplay_hbomber_shape1_yoffset")
                        }
                },
                {
                        cmp::create_circle(cfg::real("gameplay_hbomber_shape_radius")),
                        {
                                cfg::real("gameplay_hbomber_shape2_xoffset"),
                                cfg::real("gameplay_hbomber_shape2_yoffset")
                        },
                },
                {
                        cmp::create_circle(cfg::real("gameplay_hbomber_shape_radius")),
                        {
                                cfg::real("gameplay_hbomber_shape3_xoffset"),
                                cfg::real("gameplay_hbomber_shape3_yoffset")
                        }
                } 
        });
        auto flash_flag = make_shared<bool>(false);

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::SHIPS;
        auto appearance_base = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER)); 
        auto appearance_flash = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER_FLASH)); 

        auto appearance = cmp::create_bin_proxy_appr(
                flash_flag,
                appearance_flash,
                appearance_base);

        // Movement components.
        // --------------------
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                -cfg::real("gameplay_wide_offscreen_bound") - 1.0,
                cfg::integer("gfx_screen_w") + cfg::real("gameplay_wide_offscreen_bound") + 1.0,
                cfg::integer("gfx_screen_h") + cfg::real("gameplay_wide_offscreen_bound") + 1.0);

        // Arms components.
        // ----------------
        auto upgrades = cmp::create_upgrades(0, 0, 0, 0);
        auto weapon_beh = cmp::create_complex_weapon_beh({
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_hbomber_bullet_interval"),
                                        cfg::real("gameplay_hbomber_bullet_interval"),
                                        +cfg::real("gfx_hbomber_gun_xoffset"),
                                        cfg::real("gfx_hbomber_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_bullet(
                                        cfg::real("gameplay_hbomber_bullet_interval"),
                                        cfg::real("gameplay_hbomber_bullet_interval"),
                                        -cfg::real("gfx_hbomber_gun_xoffset"),
                                        cfg::real("gfx_hbomber_gun_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_bullet_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_hbomber_missile_interval"),
                                        cfg::real("gameplay_hbomber_missile_interval"),
                                        +cfg::real("gfx_hbomber_rl_xoffset"),
                                        cfg::real("gfx_hbomber_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")),
                        cmp::create_period_missile(
                                        cfg::real("gameplay_hbomber_missile_interval"),
                                        cfg::real("gameplay_hbomber_missile_interval"),
                                        -cfg::real("gfx_hbomber_rl_xoffset"),
                                        cfg::real("gfx_hbomber_rl_yoffset"),
                                        0.0, 1.0,
                                        cfg::real("gameplay_missile_lin_vel")) });

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile( 
                cmp::pain_team::ENEMY,
                cmp::coll_class::SHIP,
                false,
                cfg::real("gameplay_hbomber_damage"));

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(
                        cfg::real("gameplay_hbomber_max_health"),
                        cfg::real("gameplay_hbomber_max_shield"));
        vector<shared_ptr<cmp::reaction>> reactions {
                cmp::create_debris_reaction(
                                cfg::integer("gameplay_hbomber_num_debris"),
                                {
                                        res_id::DEBRIS1,
                                        res_id::DEBRIS2,
                                        res_id::DEBRIS3,
                                        res_id::DEBRIS4,
                                        res_id::DEBRIS5
                                },
                                cfg::real("gameplay_debris_min_vel"),
                                cfg::real("gameplay_debris_max_vel"),
                                cfg::real("gameplay_debris_min_theta"),
                                cfg::real("gameplay_debris_max_theta"),
                        /* explode = */ false,
                        /* randomize = */ true),
                cmp::create_explosion_sequence_reaction(
                                cfg::integer("gameplay_hbomber_num_explosions"),
                                cfg::real("gfx_explosion_seq_min_delay"),
                                cfg::real("gfx_explosion_seq_min_delay"))
        };

        bernoulli_distribution drop_health(cfg::real("gameplay_hbomber_health_drop_dist"));
        if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

        bernoulli_distribution drop_battery(cfg::real("gameplay_hbomber_battery_drop_dist"));
        if(drop_battery(rnd::engine))
                reactions.push_back(cmp::create_battery_drop_reaction());

        bernoulli_distribution drop_bul_up(cfg::real("gameplay_hbomber_gun_up_drop_dist"));
        if(drop_bul_up(rnd::engine))
                reactions.push_back(cmp::create_bullet_upgrade_drop_reaction());

        bernoulli_distribution drop_mis_up(cfg::real("gameplay_hbomber_ml_up_drop_dist"));
        if(drop_mis_up(rnd::engine))
                reactions.push_back(cmp::create_missile_upgrade_drop_reaction());

        auto on_death = cmp::create_complex_reaction(reactions);
        shared_ptr<cmp::timer> ttl;

        // Fx Components.
        // --------------
        auto fxs = cmp::create_complex_fx({
            cmp::create_smoke_when_hurt(
                cmp::create_const_int_timer(cfg::real("gameplay_smoke_interval")),
                cfg::real("gameplay_smoke_health_ratio_threshold")),
            cmp::create_flash_when_hurt(flash_flag, cfg::real("gfx_pain_flash_timer"))
        });

        // Score components.
        // -----------------
        auto score = make_shared<double>();
        auto sc = cmp::score_class::ENEMY_HEAVY_BOMBER;

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register the components.
        // ------------------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
        _arms_system.add_node({ id, orientation, weapon_beh, upgrades, nqueue });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "h_bomber" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _score_system.add_node({ id, sc, score, wellness });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_common_pickup(
                double x, double y,
                double vx, double vy,
                pickup_type type) {

        // Helpers.
        // --------
        bernoulli_distribution dir_dist;

        uniform_real_distribution<double> mv_dist(
                        cfg::real("gameplay_pickup_min_vel"),
                        cfg::real("gameplay_pickup_max_vel"));
        double base_vx = mv_dist(rnd::engine);
        double base_vy = mv_dist(rnd::engine);
        double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
        double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

        uniform_real_distribution<double> rot_dist(
                        cfg::real("gameplay_pickup_min_theta"),
                        cfg::real("gameplay_pickup_max_theta"));
        double base_av = rot_dist(rnd::engine);
        double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

        // Type dependent components.
        // --------------------------
        res_id image_id = res_id::HEALTH;
        shared_ptr<cmp::pickup_profile> pp;
        switch(type) {
        case pickup_type::health:
                image_id = res_id::HEALTH;
                pp = cmp::create_health_pickup_profile(cfg::real("gameplay_pickup_health_amount"));
                break;
        case pickup_type::battery:
                image_id = res_id::BATTERY;
                pp = cmp::create_battery_pickup_profile(cfg::real("gameplay_pickup_shield_amount"));
                break;
        case pickup_type::bullet_up:
                image_id = res_id::B_UPGRADE;
                pp = cmp::create_bullet_upgrade_pickup_profile();
                break;
        case pickup_type::missile_up:
                image_id = res_id::M_UPGRADE;
                pp = cmp::create_missile_upgrade_pickup_profile();
                break;

        default:
                break;
        }

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, 0.0);
        auto shape = cmp::create_circle(cfg::real("gameplay_pickup_shape_radius"));

        // Drawing components.
        auto draw_plane = cmp::draw_plane::FX;
        auto appearance = cmp::create_static_bmp(
                        _resman.get_bitmap(image_id));

        // Movement components.
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(0.0, 0.0, cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"));
        auto dynamics = cmp::create_complex_dynamics({
                cmp::create_const_velocity_dynamics(
                        vx + base_vx * mul_vx,
                        vy + base_vy * mul_vy),
                cmp::create_const_ang_vel_dynamics(
                        base_av * mul_av) });

        // Collision components.
        auto coll_queue = cmp::create_coll_queue();
        auto cp = cmp::create_collision_profile(
                cmp::pain_team::NONE,
                cmp::coll_class::PICKUP,
                false,
                -1);

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics });
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
        _collision_system.add_node({ id, id, orientation, cp, pp, shape, coll_queue, "pickup" });

        return id;
}

uint64_t entity_factory::create_health_pickup(double x, double y, double vx, double vy) {
        return create_common_pickup(x, y, vx, vy, pickup_type::health);
}

uint64_t entity_factory::create_battery_pickup(double x, double y, double vx, double vy) {
        return create_common_pickup(x, y, vx, vy, pickup_type::battery);
}

uint64_t entity_factory::create_bullet_upgrade_pickup(double x, double y, double vx, double vy) {
        return create_common_pickup(x, y, vx, vy, pickup_type::bullet_up);
}

uint64_t entity_factory::create_missile_upgrade_pickup(double x, double y, double vx, double vy) {
        return create_common_pickup(x, y, vx, vy, pickup_type::missile_up);
}

uint64_t entity_factory::create_missile(
                double x, double y,
                double dir_x, double dir_y,
                double lin_vel,
                unsigned upgrade_lvl,
                bool enemy,
                uint64_t origin_id) {

        // Interpret direction.
        // --------------------
        const double vx = dir_x * lin_vel;
        const double vy = dir_y * lin_vel;
        const double ax = 0;
        const double ay = (vy > 0)
                        ? +cfg::real("gameplay_missile_lin_acc")
                        : -cfg::real("gameplay_missile_lin_acc");
        const double theta = atan2(dir_y, dir_x);

        // Helpers.
        // ----------
        const double missile_health = 1.0;
        const double missile_shield = 0.0;

        double damage_base = cfg::real("gameplay_missile_damage_base");
        double multiplier = upgrade_lvl;
        double damage = damage_base * multiplier;

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, theta); 
        auto shape = cmp::create_circle(cfg::real("gameplay_missile_shape_radius"));
        shared_ptr<cmp::upgrades> upgrades;

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::PROJECTILES;
        auto appearance = cmp::create_static_bmp(
                        _resman.get_bitmap(res_id::MISSILE));

        // Movement components.
        // --------------------
        auto dynamics = cmp::create_const_acc_dynamics(vx, vy, ax, ay);
        auto movement_bounds = shared_ptr<cmp::bounds>(); 
        auto life_bounds = cmp::create_bounds(0.0, 0.0, cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"));

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue(); 
        auto pain_team = enemy ? cmp::pain_team::ENEMY : cmp::pain_team::PLAYER;
        auto cp = cmp::create_collision_profile( 
                pain_team,
                cmp::coll_class::PROJECTILE,
                true,
                damage);

        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(missile_health, missile_shield);
        auto on_death = cmp::create_complex_reaction({
                        cmp::create_debris_reaction(
                                        cfg::integer("gameplay_missile_num_debris"),
                                        {
                                                res_id::DEBRIS1,
                                                res_id::DEBRIS2,
                                                res_id::DEBRIS3,
                                                res_id::DEBRIS4,
                                                res_id::DEBRIS5
                                        },
                                        cfg::real("gameplay_debris_min_vel"),
                                        cfg::real("gameplay_debris_max_vel"),
                                        cfg::real("gameplay_debris_min_theta"),
                                        cfg::real("gameplay_debris_max_theta"),
                                        /* explode = */ false,
                                        /* randomize = */ true),
                        cmp::create_explosion_sequence_reaction(1,
                                        cfg::real("gfx_explosion_seq_min_delay"),
                                        cfg::real("gfx_explosion_seq_min_delay")) });

        shared_ptr<cmp::timer> ttl;

        // Fx components.
        // --------------
        auto fxs = cmp::create_period_smoke(
                        cfg::real("gameplay_smoke_interval"),
                        cfg::real("gameplay_smoke_interval"));

        // Sound components.
        // -----------------
        auto nqueue = make_shared<cmp::noise_queue>();

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics }); 
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds}); 
        _collision_system.add_node({ id, origin_id, orientation, cp, pp, shape, coll_queue, "missile" });
        _pain_system.add_node({ id, coll_queue, cp, wellness });
        _wellness_system.add_node({ id, on_death, orientation, shape, dynamics, nqueue, wellness, ttl });
        _fx_system.add_node({ id, appearance, orientation, shape, wellness, fxs });
        _sound_system.add_node({ id, nqueue });

        return id;
}

uint64_t entity_factory::create_bullet(
                double x, double y,
                double dir_x, double dir_y,
                double lin_vel,
                unsigned upgrade_lvl,
                bool enemy,
                uint64_t origin_id) {

        // Interpret direction.
        // --------------------
        const double vx = dir_x * lin_vel;
        const double vy = dir_y * lin_vel;
        const double theta = atan2(dir_y, dir_x);

        // Helpers.
        // --------
        double bullet_health = 1.0;
        double bullet_shield = 0.0;

        double damage_base = cfg::real("gameplay_bullet_damage_base");
        double multiplier = upgrade_lvl;
        double damage = damage_base * multiplier;

        res_id image;
        switch(upgrade_lvl) {
        case 1:
                image = res_id::BULLET_1;
                break;
        case 2:
                image = res_id::BULLET_2;
                break;
        case 3:
                image = res_id::BULLET_3;
                break;
        case 4:
                image = res_id::BULLET_4;
                break;
        case 5:
                image = res_id::BULLET_5;
                break;
        default:
                cerr << "Level = " << (unsigned)upgrade_lvl << " :O";
                exit(1);
        }

        // Common components.
        // ------------------
        uint64_t id = ++_last_id;
        auto orientation = cmp::create_orientation(x, y, theta); 
        auto shape = cmp::create_circle(cfg::real("gameplay_bullet_shape_radius"));
        shared_ptr<cmp::upgrades> upgrades;

        // Drawing components.
        // -------------------
        auto draw_plane = cmp::draw_plane::PROJECTILES;
        auto appearance = cmp::create_static_bmp(_resman.get_bitmap(image));

        // Movement components.
        // --------------------
        auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
        auto movement_bounds = shared_ptr<cmp::bounds>();
        auto life_bounds = cmp::create_bounds(0.0, 0.0, cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"));

        // Collision components.
        // ---------------------
        auto coll_queue = cmp::create_coll_queue(); 
        auto pain_team = enemy ? cmp::pain_team::ENEMY : cmp::pain_team::PLAYER;
        auto cp = cmp::create_collision_profile( 
                pain_team,
                cmp::coll_class::PROJECTILE,
                true,
                damage);
  
        shared_ptr<cmp::pickup_profile> pp;

        // Wellness components.
        // --------------------
        auto wellness = cmp::create_wellness(bullet_health, bullet_shield);
        shared_ptr<cmp::timer> ttl;
        shared_ptr<cmp::reaction> on_death;

        // Register nodes.
        // ---------------
        _drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, dynamics }); 
        _movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds }); 
        _collision_system.add_node({ id, origin_id, orientation, cp, pp, shape, coll_queue, "bullet" }); 
        _pain_system.add_node({ id, coll_queue, cp, wellness });

        return id;
}

