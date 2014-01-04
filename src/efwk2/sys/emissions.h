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

#include "../comm.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

#include "../cmp/fx.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"

namespace efwk
{

template <class Shape>
void emit_impl(fx_emit_smoke& eff,
               const orientation& ori,
               const Shape& shp,
               double dt,
               comm_bus& cbus)
{
        eff.cdown.update(dt);
        if (eff.cdown.trigger()) {
                double x, y;
                std::tie(x, y) = ori.interpolate_loc(0);
                cbus.smoke_reqs.push({ x, y });
        }
}

template <class Shape>
void emit_impl(fx_emit_spark& eff,
               const orientation& ori,
               const Shape& shp,
               double dt,
               comm_bus& cbus)
{
        eff.cdown.update(dt);
        if (eff.cdown.trigger()) {
                std::cout << "Emitting spark" << std::endl;
        }
}

template <class T>
using IsEmitting = TmpAll<HasFxEmit<T>, HasOrientation<T>, HasShape<T>>;

template <class Entity>
typename std::enable_if<IsEmitting<Entity>::value, void>::type
emit(Entity& ent, double dt, comm_bus& cbus)
{
        emit_impl(ent.eff, ent.ori, ent.shp, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsEmitting<Entity>::value, void>::type
emit(Entity& ent, double dt, comm_bus& cbus) {}

}

#endif
