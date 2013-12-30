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

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <algorithm>
#include <functional>
#include <type_traits>

#include "../cmp/dynamics.h"
#include "../cmp/orientation.h"

namespace efwk
{

template <class Dynamics>
void move_impl(Dynamics& dyn, orientation& ori, double dt)
{
        dyn.update(dt);
        ori.shift(dyn.vx * dt, dyn.vy * dt);
}

template <class T>
using IsMovable = TmpAll<HasDynamics<T>, HasOrientation<T>>;

template<class Entity>
typename std::enable_if<IsMovable<Entity>::value, void>::type
move(Entity& ent, double dt)
{
        move_impl(ent.dyn, ent.ori, dt);
}

template<class Entity>
typename std::enable_if<!IsMovable<Entity>::value, void>::type
move(Entity&, double) {}

}

#endif
