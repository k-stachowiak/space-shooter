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

namespace efwk
{

// Logic implementation.
// =====================

template <class Entity>
void bind_movement_impl(Entity& ent)
{
        auto& ori = ent.ori;
        const auto& mbnd = ent.mbnd;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(1.0);

        if (!point_in_bounds(x, y, mbnd))
                ori.cancel_move();
                // TODO: enable sliding based on the old implementation.
}

template <class Entity>
void bind_life_impl(const Entity& ent, comm_bus& cbus)
{
        long id = ent.id;
        const auto& ori = ent.ori;
        const auto& lbnd = ent.lbnd;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(1.0);

        if (!point_in_bounds(x, y, lbnd))
                cbus.dels.push(id);
}

// Logic dispatch.
// ===============

// Move binding.
// -------------

template <class T>
using IsMoveBoundable = TmpAll<HasOrientation<T>, HasMoveBounds<T>>;

template <class Entity>
typename std::enable_if<IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity& ent) { bind_movement_impl(ent); }

template <class Entity>
typename std::enable_if<!IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity&) {}

// Life binding.
// -------------

template <class T>
using IsLifeBoundable = TmpAll<HasOrientation<T>, HasLifeBounds<T>>;

template <class Entity>
typename std::enable_if<IsLifeBoundable<Entity>::value, void>::type
bind_life(const Entity& ent, comm_bus& cbus) { bind_life_impl(ent, cbus); }

template <class Entity>
typename std::enable_if<!IsLifeBoundable<Entity>::value, void>::type
bind_life(const Entity&, comm_bus&) {}

}

#endif
