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

#ifndef FX_DEATHSPAWN_H
#define FX_DEATHSPAWN_H

#include "spawh_common.h"

#include "../../cmp/wellness.h"
#include "../../cmp/death_spawner.h"
#include "../../cmp/orientation.h"

#include "../../comm.h"

namespace efwk
{

template <class Wellness, class Shape>
void fx_death_spawn_impl(const long id,
                         const Wellness& wlns,
                         const death_spawner& dspwn,
                         const orientation& ori,
                         const Shape& shp,
                         comm_bus& cbus)
{
        if (wlns.alive())
                return;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);

        // Spawn explosions.
        for (int i = 0; i < dspwn.num_explosions; ++i) {
                double rx, ry;
                std::tie(rx, ry) = random_point(shp, ori);
                cbus.expl_reqs.push({ rx, ry }, 0.125 * i);
        }

        // Spawn sparks.
        static std::bernoulli_distribution spark_dir_distr(0.5);
        static std::uniform_real_distribution<double> spark_vel_distr(50, 100);
        static std::uniform_real_distribution<double> spark_bri_distr(0.5, 1.0);

        for (int i = 0; i < dspwn.num_sparks; ++i) {
                double vel_x, vel_y;
                std::tie(vel_x, vel_y) = gen_rnd_vel(
                                spark_dir_distr, spark_dir_distr,
                                spark_vel_distr, spark_vel_distr);
                const double bri = spark_bri_distr(rnd::engine);
                cbus.spark_reqs.push({
                        x, y, vel_x, vel_y,
                        {{ bri, bri, bri }},
                        0.2
                });
        }

        // Spawn pickups.
        static std::bernoulli_distribution pickup_dir_distr(0.5);
        static std::uniform_real_distribution<double> pickup_vel_distr(50, 100);
        static std::uniform_real_distribution<double> pickup_bri_distr(0.5, 1.0);

        std::bernoulli_distribution health_drop_distr(dspwn.health_prob);
        if (health_drop_distr(rnd::engine)) {
                double vel_x, vel_y;
                std::tie(vel_x, vel_y) = gen_rnd_vel(
                                pickup_dir_distr, pickup_dir_distr,
                                pickup_vel_distr, pickup_vel_distr);
                cbus.health_reqs.push({ id, x, y, vel_x, vel_y });
        }

        std::bernoulli_distribution bullupgr_drop_distr(dspwn.bullupgr_prob);
        if (bullupgr_drop_distr(rnd::engine)) {
                double vel_x, vel_y;
                std::tie(vel_x, vel_y) = gen_rnd_vel(
                                pickup_dir_distr, pickup_dir_distr,
                                pickup_vel_distr, pickup_vel_distr);
                cbus.bullupgr_reqs.push({ id, x, y, vel_x, vel_y });
        }

        std::bernoulli_distribution missupgr_drop_distr(dspwn.missupgr_prob);
        if (missupgr_drop_distr(rnd::engine)) {
                double vel_x, vel_y;
                std::tie(vel_x, vel_y) = gen_rnd_vel(
                                pickup_dir_distr, pickup_dir_distr,
                                pickup_vel_distr, pickup_vel_distr);
                cbus.missupgr_reqs.push({ id, x, y, vel_x, vel_y });
        }
}

template <class T>
using IsDeathSpawnable = TmpAll<HasWellness<T>,
                                HasDeathSpawner<T>,
                                HasOrientation<T>,
                                HasShape<T>>;

template <class Entity>
typename std::enable_if<IsDeathSpawnable<Entity>::value, void>::type
fx_death_spawn(const Entity& ent, comm_bus& cbus)
{
        fx_death_spawn_impl(ent.id, ent.wlns, ent.dspwn, ent.ori, ent.shp, cbus);
}

template <class Entity>
typename std::enable_if<!IsDeathSpawnable<Entity>::value, void>::type
fx_death_spawn(const Entity& ent, comm_bus& cbus) {}

}

#endif
