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

#include <random>
using std::uniform_real_distribution;
using std::uniform_int_distribution;

#include "../misc/config.h"
#include "../misc/rand.h"

#include "cmp_misc.h"
#include "cmp_weapon_beh.h"

namespace cmp {

class complex_weapon_beh : public weapon_beh {
        vector<shared_ptr<weapon_beh>> _wbs;
public:
        complex_weapon_beh(vector<shared_ptr<weapon_beh>> wbs) : _wbs(wbs) {}

        void update(uint64_t id,
                        upgrades& up,
                        double dt,
                        double x, double y,
                        noise_queue& nqueue,
                        comm::msg_queue& msgs) {

                for(auto& wb : _wbs) {
                        wb->update(id, up, dt, x, y, nqueue, msgs);
                }
        }

        void input(map<int, bool>& keys) {
                for(auto& wb: _wbs) {
                        wb->input(keys);
                }
        }
};

class period_bullet : public weapon_beh {
        double _dt_min;
        double _dt_max;
        double _x_off;
        double _y_off;
        double _x_dir;
        double _y_dir;
        double _lin_vel;
        double _counter;

        void init_counter(double remainder = 0.0) {
                uniform_real_distribution<double> distr(_dt_min, _dt_max);
                _counter = distr(rnd::engine) - remainder;
        }

public:
        period_bullet(
                        double dt_min, double dt_max,
                        double x_off, double y_off,
                        double x_dir, double y_dir,
                        double lin_vel)
        : _dt_min(dt_min)
        , _dt_max(dt_max)
        , _x_off(x_off)
        , _y_off(y_off)
        , _x_dir(x_dir)
        , _y_dir(y_dir)
        , _lin_vel(lin_vel)
               , _counter(0) {
                init_counter();
        }

        void update(uint64_t id,
                        upgrades& up,
                        double dt,
                        double x, double y,
                        noise_queue& nqueue,
                        comm::msg_queue& msgs) {
                _counter -= dt;
                if(_counter <= 0.0) {
                        init_counter(-_counter);
                        up.tick_down_gun();
                        nqueue.push(res_id::BULLET_SHOOT);
                        msgs.push(comm::create_spawn_bullet(
                                x + _x_off, y + _y_off,
                                _x_dir, _y_dir, _lin_vel,
                                up.gun_lvl(),
                                true,
                                id));
                }
        }

        void input(map<int, bool>& keys) {}
};

class period_missile : public weapon_beh {
        double _dt_min;
        double _dt_max;
        double _x_off;
        double _y_off;
        double _x_dir;
        double _y_dir;
        double _lin_vel;
        double _counter;

        void init_counter(double remainder = 0.0) {
                uniform_real_distribution<double> distr(_dt_min, _dt_max);
                _counter = distr(rnd::engine) - remainder;
        }

public:
        period_missile(
                        double dt_min, double dt_max,
                        double x_off, double y_off,
                        double x_dir, double y_dir,
                        double lin_vel)
        : _dt_min(dt_min)
        , _dt_max(dt_max)
        , _x_off(x_off)
        , _y_off(y_off)
        , _x_dir(x_dir)
        , _y_dir(y_dir)
        , _lin_vel(lin_vel)
        {
                init_counter();
        }

        void update(uint64_t id,
                        upgrades& up,
                        double dt,
                        double x, double y,
                        noise_queue& nqueue,
                        comm::msg_queue& msgs) {
                _counter -= dt;
                if(_counter <= 0.0) {
                        init_counter(-_counter);
                        up.tick_down_rl();
                        nqueue.push(res_id::MISSILE_SHOOT);
                        msgs.push(comm::create_spawn_missile(
                                x + _x_off, y + _y_off,
                                _x_dir, _y_dir, _lin_vel,
                                up.rl_lvl(),
                                true,
                                id));
                }
        }

        void input(map<int, bool>& keys) {}
};

class player_controlled_weapon_beh : public weapon_beh {

        bool _prev_left;
        weapon _minigun;
        weapon _rpg;

public:
        player_controlled_weapon_beh()
        : _prev_left(false)
        , _minigun(cfg::real("gameplay_player_gun_interval"))
        , _rpg(cfg::real("gameplay_player_rl_interval"))
        {}

        void update(uint64_t id,
                        upgrades& up,
                        double dt,
                        double x, double y,
                        noise_queue& nqueue,
                        comm::msg_queue& msgs) {

                // Minigun fire.
                // -------------
                if(_minigun.update(dt)) {
                        up.tick_down_gun();
                        if(_prev_left) {
                                _prev_left = false;
                                nqueue.push(res_id::BULLET_SHOOT);
                                msgs.push(comm::create_spawn_bullet(
                                                x + cfg::real("gfx_player_gun_offset"), y,
                                                0.0, -1.0, cfg::real("gameplay_bullet_lin_vel"),
                                                up.gun_lvl(),
                                                false,
                                                id));
                        } else {
                                _prev_left = true;
                                nqueue.push(res_id::BULLET_SHOOT);
                                msgs.push(comm::create_spawn_bullet(
                                                x - cfg::real("gfx_player_gun_offset"), y,
                                                0.0, -1.0, cfg::real("gameplay_bullet_lin_vel"),
                                                up.gun_lvl(),
                                                false,
                                                id));
                        }
                }

                // Rocket launcher fire.
                // ---------------------
                if(_rpg.update(dt)) {
                        up.tick_down_rl();
                        nqueue.push(res_id::MISSILE_SHOOT);
                        msgs.push(comm::create_spawn_missile(
                                        x + cfg::real("gfx_player_rl_offset"), y,
                                        0.0, -1.0, cfg::real("gameplay_bullet_lin_vel"),
                                        up.rl_lvl(),
                                        false,
                                        id));
                        msgs.push(comm::create_spawn_missile(
                                        x - cfg::real("gfx_player_rl_offset"), y,
                                        0.0, -1.0, cfg::real("gameplay_bullet_lin_vel"),
                                        up.rl_lvl(),
                                        false,
                                        id));
                }
        }

        void input(map<int, bool>& keys) {
                _minigun.set_trigger(keys[ALLEGRO_KEY_Z]);
                _rpg.set_trigger(keys[ALLEGRO_KEY_X]);
        }
};

shared_ptr<weapon_beh> create_complex_weapon_beh(vector<shared_ptr<weapon_beh>> wbs) {
        return shared_ptr<weapon_beh>(new complex_weapon_beh(wbs));
}

shared_ptr<weapon_beh> create_period_bullet(
                double dt_min, double dt_max,
                double x_off, double y_off,
                double x_dir, double y_dir,
                double lin_vel) {
        return shared_ptr<weapon_beh>(new period_bullet(
                                dt_min, dt_max, x_off, y_off, x_dir, y_dir, lin_vel));
}

shared_ptr<weapon_beh> create_period_missile(
                double dt_min, double dt_max,
                double x_off, double y_off,
                double x_dir, double y_dir,
                double lin_vel) {
        return shared_ptr<weapon_beh>(new period_missile(
                                dt_min, dt_max, x_off, y_off, x_dir, y_dir, lin_vel));
}

shared_ptr<weapon_beh> create_player_controlled_weapon_beh() {
        return shared_ptr<weapon_beh>(new player_controlled_weapon_beh);
}

}
