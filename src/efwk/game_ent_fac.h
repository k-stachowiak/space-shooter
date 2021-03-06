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

#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "../efwk/comm.h"
#include "../efwk/sys/systems.h"
#include "../misc/config.h"
#include "../resources/resman.h"

enum class pickup_type {
        health,
        battery,
        bullet_up,
        missile_up
};

class entity_factory {

        uint64_t _last_id;

        res::resman const& _resman;

        sys::movement_system&    _movement_system;
        sys::collision_system&   _collision_system;
        sys::arms_system&        _arms_system;
        sys::pain_system&        _pain_system;
        sys::wellness_system&    _wellness_system;
        sys::fx_system&          _fx_system;
        sys::drawing_system&     _drawing_system;
        sys::score_system&       _score_system;
        sys::pickup_system&      _pickup_system;
        sys::input_system&       _input_system;
        sys::hud_system&         _hud_system;
        sys::sound_system&       _sound_system;

public:
        entity_factory(
                res::resman const& rm,
                sys::movement_system&    movement_system,
                sys::collision_system&   collision_system,
                sys::arms_system&        arms_system,
                sys::pain_system&        pain_system,
                sys::wellness_system&    wellness_system,
                sys::fx_system&          fx_system,
                sys::drawing_system&     drawing_system,
                sys::score_system&       score_system,
                sys::pickup_system&      pickup_system,
                sys::input_system&       input_system,
                sys::hud_system&         hud_system,
                sys::sound_system&       sound_system)
        : _last_id(0)
        , _resman(rm)
        , _movement_system (movement_system)
        , _collision_system (collision_system)
        , _arms_system (arms_system)
        , _pain_system (pain_system)
        , _wellness_system (wellness_system)
        , _fx_system (fx_system)
        , _drawing_system (drawing_system)
        , _score_system (score_system) 
        , _pickup_system (pickup_system)
        , _input_system (input_system)  
        , _hud_system(hud_system)
        , _sound_system(sound_system)
        {}

        uint64_t create_explosion(double x, double y);
        uint64_t create_smoke(double x, double y, comm::smoke_size size);

        uint64_t create_debris(double x, double y,
                        double bvx, double bvy,
                        double vmin, double vmax,
                        double theta_min, double theta_max,
                        res::res_id bmp,
                        bool explode,
                        uint64_t origin_id);

        uint64_t create_star();
		void create_star_drop_id();
        uint64_t create_star_xy(double x, double y);
        uint64_t create_player_ship(double x, double y);

        uint64_t create_light_fighter_dyn(double x, double y, std::shared_ptr<cmp::dynamics> dynamics);
        uint64_t create_heavy_fighter_dyn(double x, double y, std::shared_ptr<cmp::dynamics> dynamics);
        uint64_t create_light_bomber_dyn(double x, double y, std::shared_ptr<cmp::dynamics> dynamics);
        uint64_t create_heavy_bomber_dyn(double x, double y, std::shared_ptr<cmp::dynamics> dynamics);

        uint64_t create_common_pickup(double x, double y, double vx, double vy, pickup_type type);

        uint64_t create_health_pickup(double x, double y, double vx, double vy);
        uint64_t create_battery_pickup(double x, double y, double vx, double vy);
        uint64_t create_bullet_upgrade_pickup(double x, double y, double vx, double vy);
        uint64_t create_missile_upgrade_pickup(double x, double y, double vx, double vy);

        uint64_t create_missile(
                        double x, double y,
                        double dir_x, double dir_y,
                        double lin_vel,
                        unsigned upgrade_lvl,
                        bool enemy,
                        uint64_t origin_id);

        uint64_t create_bullet(
                        double x, double y,
                        double dir_x, double dir_y,
                        double lin_vel,
                        unsigned upgrade_lvl,
                        bool enemy,
                        uint64_t origin_id);
};

#endif
