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

#ifndef PICKUP_H
#define PICKUP_H

#include "../cmp/upgrade.h"

namespace efwk
{

template <class Wellness>
void pick_impl(const coll_queue& collq,
               Wellness& wlns,
               upgrade& upgr,
               comm_bus& cbus)
{
        collq.for_each_report([&wlns, &upgr, &cbus] (const coll_report& cr) mutable
        {
                if (cr.collc == coll_class::pickup) {
                        wlns.hurt(-cr.collp.health);

                        if (cr.collp.bullupgr)
                                upgr.upgrade_gun();

                        if (cr.collp.missupgr)
                                upgr.upgrade_missile();

                        cbus.del_reqs.push(cr.id);
                }
        });
}

template <class T>
using IsPicker = TmpAll<HasWellness<T>,
                        HasCollisionQueue<T>,
                        HasUpgrade<T>>;

template <class Entity>
typename std::enable_if<IsPicker<Entity>::value, void>::type
pick(Entity& ent, comm_bus& cbus)
{
        pick_impl(ent.cqueue, ent.wlns, ent.upgr, cbus);
}

template <class Entity>
typename std::enable_if<!IsPicker<Entity>::value, void>::type
pick(Entity&, comm_bus&) {}

}

#endif
