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

#ifndef FX_TIMESPAWN_H
#define FX_TIMESPAWN_H

#include "../../comm.h"
#include "../../tmp/sfinae.h"
#include "../../tmp/traits.h"

#include "../../cmp/orientation.h"
#include "../../cmp/shape.h"
#include "../../cmp/time_spawner.h"

namespace efwk
{

template <class Shape>
void fx_time_spawn_impl(time_spawner_spark& tspwn,
                        const orientation& ori,
                        const Shape& shp,
                        const double dt,
                        comm_bus& cbus)
{
        if (tspwn.state == cmp_state::disabled)
                return;

        std::bernoulli_distribution spark_dir_distr { 0.5 };
        std::uniform_real_distribution<double> spark_vel_distr { 30, 70 };
        std::uniform_real_distribution<double> spark_bri_distr { 0.25, 1.0 };

        double vel_x, vel_y;
        std::tie(vel_x, vel_y) = gen_rnd_vel(
                        spark_dir_distr, spark_dir_distr,
                        spark_vel_distr, spark_vel_distr);
        const double bri = spark_bri_distr(rnd::engine);

        tspwn.cdown.update(dt);
        if (tspwn.cdown.trigger()) {
                double x, y;
                std::tie(x, y) = ori.interpolate_loc(0);
                cbus.spark_reqs.push({
                        x, y, vel_x, vel_y,
                        {{ bri, bri, bri }},
                        0.5
                });
        }
}

template <class Shape>
void fx_time_spawn_impl(time_spawner_smoke& tspwn,
                       const orientation& ori,
                       const Shape& shp,
                       const double dt,
                       comm_bus& cbus)
{
        if (tspwn.state == cmp_state::disabled)
                return;

        tspwn.cdown.update(dt);
        if (tspwn.cdown.trigger()) {
                double x, y;
                std::tie(x, y) = ori.interpolate_loc(0);
                cbus.smoke_reqs.push({ x, y });
        }
}

template <class Shape>
void fx_time_spawn_impl(time_spawner_compound& tspwn,
                        const orientation& ori,
                        const Shape& shp,
                        const double dt,
                        comm_bus& cbus)
{
        fx_time_spawn_impl(tspwn.spark, ori, shp, dt, cbus);
        fx_time_spawn_impl(tspwn.smoke, ori, shp, dt, cbus);
}

template <class T>
using IsFxSpawner = TmpAll<HasTimeSpawner<T>, HasOrientation<T>, HasShape<T>>;

template <class Entity>
typename std::enable_if<IsFxSpawner<Entity>::value, void>::type
fx_time_spawn(Entity& ent, const double dt, comm_bus& cbus)
{
        fx_time_spawn_impl(ent.tspwn, ent.ori, ent.shp, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsFxSpawner<Entity>::value, void>::type
fx_time_spawn(Entity& ent, const double dt, comm_bus& cbus) {}

}

#endif
