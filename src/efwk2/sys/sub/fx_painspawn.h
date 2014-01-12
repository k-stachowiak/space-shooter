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

#ifndef FX_PAINSPAWN_H
#define FX_PAINSPAWN_H

#include "../../comm.h"
#include "../../tmp/sfinae.h"
#include "../../tmp/traits.h"

#include "../../cmp/orientation.h"
#include "../../cmp/wellness.h"
#include "../../cmp/shape.h"
#include "../../cmp/pain_spawner.h"

namespace efwk
{

template <class Wellness, class Shape>
void fx_pain_spawn_impl(const Wellness& wlns,
                        pain_spawner& pspwn,
                        const orientation& ori,
                        const Shape& shp,
                        const double dt,
                        comm_bus& cbus)
{
        if (pspwn.smoke_state == cmp_state::disabled)
                return;

        double ratio = wlns.get_health() / wlns.get_health_max();
        if (ratio > pspwn.smoke_threshold)
                return;

        pspwn.smoke_cdown.update(dt);
        if (pspwn.smoke_cdown.trigger()) {
                double x, y;
                std::tie(x, y) = random_point(shp, ori);
                cbus.smoke_reqs.push({ x, y });
        }
}

template <class T>
using IsPainSpawnable = TmpAll<HasWellness<T>,
                               HasPainSpawner<T>,
                               HasOrientation<T>,
                               HasShape<T>>;

template <class Entity>
typename std::enable_if<IsPainSpawnable<Entity>::value, void>::type
fx_pain_spawn(Entity& ent, const double dt, comm_bus& cbus)
{
        fx_pain_spawn_impl(ent.wlns, ent.pspwn, ent.ori, ent.shp, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsPainSpawnable<Entity>::value, void>::type
fx_pain_spawn(Entity& ent, const double dt, comm_bus& cbus) {}

}

#endif
