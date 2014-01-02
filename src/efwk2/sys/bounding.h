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

#ifndef BOUNDING_H
#define BOUNDING_H

#include "../comm.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

#include "../cmp/orientation.h"
#include "../cmp/bounds.h"

namespace efwk
{

// Logic implementation.
// =====================

inline
void bind_movement_impl(orientation& ori, const move_bounds& mbnd)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(1.0);

        if (!point_in_bounds(x, y, mbnd))
                ori.cancel_move();
                // TODO: enable sliding based on the old implementation.
}

inline
void bind_life_impl(long id,
                    const orientation& ori,
                    const life_bounds& lbnd,
                    comm_bus& cbus)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(1.0);

        if (!point_in_bounds(x, y, lbnd))
                cbus.del_reqs.push(id);
}

inline
void bind_time_impl(long id, double& ttl, double dt, comm_bus& cbus)
{
        if ((ttl -= dt) <= 0) {
                cbus.del_reqs.push(id);
        }
}

// Logic dispatch.
// ===============

// Move binding.
// -------------

template <class T>
using IsMoveBoundable = TmpAll<HasOrientation<T>, HasMoveBounds<T>>;

template <class Entity>
typename std::enable_if<IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity& ent)
{
        bind_movement_impl(ent.ori, ent.mbnd);
}

template <class Entity>
typename std::enable_if<!IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity&) {}

// Life binding.
// -------------

template <class T>
using IsLifeBoundable = TmpAll<HasOrientation<T>, HasLifeBounds<T>>;

template <class Entity>
typename std::enable_if<IsLifeBoundable<Entity>::value, void>::type
bind_life(const Entity& ent, comm_bus& cbus)
{
        bind_life_impl(ent.id, ent.ori, ent.lbnd, cbus);
}

template <class Entity>
typename std::enable_if<!IsLifeBoundable<Entity>::value, void>::type
bind_life(const Entity&, comm_bus&) {}

// Time binding.
// -------------

SFINAE__DECLARE_HAS_MEMBER(HasTimeToLive, double, ttl);

template <class T>
using IsTimeBoundable = HasTimeToLive<T>;

template <class Entity>
typename std::enable_if<IsTimeBoundable<Entity>::value, void>::type
bind_time(Entity& ent, double dt, comm_bus& cbus)
{
        bind_time_impl(ent.id, ent.ttl, dt, cbus);
}

template <class Entity>
typename std::enable_if<!IsTimeBoundable<Entity>::value, void>::type
bind_time(Entity&, double, comm_bus&) {}

}

#endif
