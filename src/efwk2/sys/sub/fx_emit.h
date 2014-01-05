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

#ifndef EMISSIONS_H
#define EMISSIONS_H

#include "../../comm.h"
#include "../../tmp/sfinae.h"
#include "../../tmp/traits.h"

#include "../../cmp/orientation.h"
#include "../../cmp/shape.h"
#include "../../cmp/emitter.h"

namespace efwk
{

template <class Shape>
void fx_emit_impl(emitter_smoke& emit,
                  const orientation& ori,
                  const Shape& shp,
                  double dt,
                  comm_bus& cbus)
{
        if (emit.state == cmp_state::disabled)
                return;

        emit.cdown.update(dt);
        if (emit.cdown.trigger()) {
                double x, y;
                std::tie(x, y) = ori.interpolate_loc(0);
                cbus.smoke_reqs.push({ x, y });
        }
}

template <class Shape>
void fx_emit_impl(emitter_spark& emit,
                  const orientation& ori,
                  const Shape& shp,
                  double dt,
                  comm_bus& cbus)
{
        if (emit.state == cmp_state::disabled)
                return;

        std::bernoulli_distribution spark_dir_distr { 0.5 };
        std::uniform_real_distribution<double> spark_vel_distr { 30, 70 };
        std::uniform_real_distribution<double> spark_bri_distr { 0.25, 1.0 };

        double dir_x = spark_dir_distr(rnd::engine) ? 1.0 : -1.0;
        double dir_y = spark_dir_distr(rnd::engine) ? 1.0 : -1.0;
        double vel_x = spark_vel_distr(rnd::engine);
        double vel_y = spark_vel_distr(rnd::engine);
        double bri = spark_bri_distr(rnd::engine);

        emit.cdown.update(dt);
        if (emit.cdown.trigger()) {
                double x, y;
                std::tie(x, y) = ori.interpolate_loc(0);
                cbus.spark_reqs.push({
                        x, y,
                        vel_x * dir_x,
                        vel_y * dir_y,
                        {{ bri, bri, bri }},
                        0.5
                });
        }
}

template <class Shape>
void fx_emit_impl(emitter_compound& emit,
                  const orientation& ori,
                  const Shape& shp,
                  double dt,
                  comm_bus& cbus)
{
        fx_emit_impl(emit.spark, ori, shp, dt, cbus);
        fx_emit_impl(emit.smoke, ori, shp, dt, cbus);
}

template <class T>
using IsFxEmitter = TmpAll<HasEmitter<T>, HasOrientation<T>, HasShape<T>>;

template <class Entity>
typename std::enable_if<IsFxEmitter<Entity>::value, void>::type
fx_emit(Entity& ent, double dt, comm_bus& cbus)
{
        fx_emit_impl(ent.emit, ent.ori, ent.shp, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsFxEmitter<Entity>::value, void>::type
fx_emit(Entity& ent, double dt, comm_bus& cbus) {}

}

#endif
