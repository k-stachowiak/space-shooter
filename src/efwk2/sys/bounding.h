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

template <class T>
using IsMoveBoundable = TmpAll<HasOrientation<T>, HasMoveBounds<T>>;

template <class Entity>
typename std::enable_if<IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity& ent)
{
        auto& ori = ent.ori;
        const auto& bnd = ent.bnd;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(1.0);

        if (!point_in_bounds(x, y, bnd))
                ori.cancel_move();
}

template <class Entity>
typename std::enable_if<!IsMoveBoundable<Entity>::value, void>::type
bind_movement(Entity&)
{
}

}

#endif
