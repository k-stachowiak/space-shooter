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

#ifndef PAIN_H
#define PAIN_H

#include "../../misc/rand.h"

#include "../cmp/wellness.h"
#include "../cmp/coll_traits.h"
#include "../cmp/orientation.h"

namespace efwk
{

// Per collision operations.
// =========================

template <class Wellness>
struct ship_reaction_func
{
        const long id;
        const coll_team& collt;
        Wellness& wlns;
        comm_bus& cbus;

        void operator()(const coll_report& cr)
        {
                // Pain case.
                // ---------
                const bool other_hurts =
                        cr.collc == coll_class::ship ||
                        cr.collc == coll_class::projectile;

                const bool other_is_enemy = cr.collt != collt;

                // Handle pain if necessary.
                if (other_hurts && other_is_enemy) {
                        wlns.hurt(cr.colld.damage);
                }

                // Pickup case.
                // ------------

                const bool other_is_pickup =
                        cr.collc == coll_class::pickup;

                const bool this_can_pick =
                        collt == coll_team::player;

                if (other_is_pickup && this_can_pick) {
                        wlns.hurt(-cr.collp.health);
                        cbus.del_reqs.push(cr.id);
                }

                // Wellness tracking.
                // ------------------

                if (!wlns.alive()) {
                        cbus.del_reqs.push(id);
                        cbus.death_events.push_back({ id, cr.score_id });
                }
        }
};

template <class Wellness>
struct projectile_reaction_func
{
        const long id;
        const coll_team& collt;
        Wellness& wlns;
        comm_bus& cbus;

        void operator()(const coll_report& cr)
        {
                const bool must_hit_other = cr.collc == coll_class::ship;
                const bool other_is_enemy = cr.collt != collt;

                if (must_hit_other && other_is_enemy) {
                        wlns.hurt(cr.colld.damage);
                }

                if (!wlns.alive()) {
                        cbus.del_reqs.push(id);
                        cbus.death_events.push_back({ id, cr.score_id });
                }
        }
};

// Logic implementation.
// =====================

template <class Wellness>
void pain_impl(const long id,
               const coll_class& collc,
               const coll_team& collt,
               const coll_queue& collq,
               Wellness& wlns,
               comm_bus& cbus)
{
        wlns.reset_hurt_flag();

        ship_reaction_func<decltype(wlns)> srf { id, collt, wlns, cbus };
        projectile_reaction_func<decltype(wlns)> prf { id, collt, wlns, cbus };

        switch (collc) {
        case coll_class::ship:
                collq.for_each_report(srf);
                break;

        case coll_class::projectile:
                collq.for_each_report(prf);
                break;

        case coll_class::pickup:
                // Note: pickups have no wellness - let's leave this like it is.
                //       schedule pickup removal in the pick logic for ship.
                break;
        }
}

// Logic dispatch.
// ===============

template <class T>
using IsPainable = TmpAll<HasWellness<T>,
                          HasCollisionTraits<T>>;

template <class Entity>
typename std::enable_if<IsPainable<Entity>::value, void>::type
pain(Entity& ent, comm_bus& cbus)
{
        pain_impl(ent.id,
                  ent.ctraits.cclass,
                  ent.ctraits.cteam,
                  ent.ctraits.cqueue,
                  ent.wlns, cbus);
}

template <class Entity>
typename std::enable_if<!IsPainable<Entity>::value, void>::type
pain(Entity&, comm_bus&) {}

}

#endif
