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

#include "../../cmp/wellness.h"
#include "../../cmp/death_spawner.h"
#include "../../cmp/orientation.h"

#include "../../comm.h"

namespace efwk
{

template <class Wellness>
void fx_death_spawn_impl(const Wellness& wlns,
                         const death_spawner& dspwn,
                         const orientation& ori,
                         comm_bus& cbus)
{
        if (wlns.alive())
                return;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);

        for (int i = 0; i < dspwn.num_explosions; ++i) {
                cbus.expl_reqs.push({ x, y }, 0.5 * i);
        }

        static std::bernoulli_distribution spark_dir_distr(0.5);
        static std::uniform_real_distribution<double> spark_vel_distr(50, 100);
        static std::uniform_real_distribution<double> spark_bri_distr(0.5, 1.0);

        for (int i = 0; i < dspwn.num_sparks; ++i) {
                const double dir_x = spark_dir_distr(rnd::engine) ? 1.0 : -1.0;
                const double dir_y = spark_dir_distr(rnd::engine) ? 1.0 : -1.0;
                const double vel_x = spark_vel_distr(rnd::engine);
                const double vel_y = spark_vel_distr(rnd::engine);
                const double bri = spark_bri_distr(rnd::engine);
                cbus.spark_reqs.push({
                        x, y,
                        vel_x * dir_x,
                        vel_y * dir_y,
                        {{ bri, bri, bri }},
                        0.2
                });
        }
}

template <class T>
using IsDeathSpawnable = TmpAll<HasWellness<T>,
                                HasDeathSpawner<T>,
                                HasOrientation<T>>;

template <class Entity>
typename std::enable_if<IsDeathSpawnable<Entity>::value, void>::type
fx_death_spawn(const Entity& ent, comm_bus& cbus)
{
        fx_death_spawn_impl(ent.wlns, ent.dspwn, ent.ori, cbus);
}

template <class Entity>
typename std::enable_if<!IsDeathSpawnable<Entity>::value, void>::type
fx_death_spawn(const Entity& ent, comm_bus& cbus) {}

}

#endif
