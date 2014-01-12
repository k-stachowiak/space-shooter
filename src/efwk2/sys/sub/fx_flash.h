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

#ifndef FX_FLASH_H
#define FX_FLASH_H

#include "../../cmp/appearance.h"
#include "../../cmp/flash.h"
#include "../../cmp/wellness.h"

namespace efwk
{

template <class Wellness, class Appr1, class Appr2>
void fx_flash_impl(const Wellness& wlns,
                   appearance_bin_proxy<Appr1, Appr2>& appr,
                   flash& flsh,
                   const double dt)
{
        if (wlns.get_hurt_flag())
                flsh.reset();

        flsh.update(dt);

        appr.state = flsh.is_flashing() ? bp_state::second : bp_state::first;
}

template <class T>
using IsFxFlashable = TmpAll<HasWellness<T>,
                             HasAppearanceBinProxy<T>,
                             HasFlash<T>>;

template <class Entity>
typename std::enable_if<IsFxFlashable<Entity>::value, void>::type
fx_flash(Entity& ent, const double dt)
{
        fx_flash_impl(ent.wlns, ent.appr, ent.flsh, dt);
}

template <class Entity>
typename std::enable_if<!IsFxFlashable<Entity>::value, void>::type
fx_flash(Entity&, const double) {}

}

#endif
