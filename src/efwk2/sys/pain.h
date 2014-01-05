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

// TODO: notice that the ones below have gotten disturbingly similar.
template <class Wellness>
struct ship_reaction_func
{
        long id;
        const coll_team& collt;
        Wellness& wlns;
        comm_bus& cbus;

        ship_reaction_func(long new_id,
                           const coll_team& new_collt,
                           Wellness& new_wlns,
                           comm_bus& new_cbus) :
                id(new_id),
                collt(new_collt),
                wlns(new_wlns),
                cbus(new_cbus)
        {}

        void operator()(const coll_report& cr)
        {
                // Analyze report.
                bool other_hurts =
                        cr.collc == coll_class::ship ||
                        cr.collc == coll_class::projectile;

                bool other_is_enemy = cr.collt != collt;

                // Handle pain if necessary.
                if (other_hurts && other_is_enemy) {
                        wlns.hurt(cr.colld.damage);
                }

                if (!wlns.alive()) {
                        cbus.del_reqs.push(id);
                        cbus.death_events.push_back({ id, cr.score_id });
                }
        }
};

template <class Wellness>
struct projectile_reaction_func
{
        long id;
        const coll_team& collt;
        Wellness& wlns;
        comm_bus& cbus;

        projectile_reaction_func(long new_id,
                                 const coll_team& new_collt,
                                 Wellness& new_wlns,
                                 comm_bus& new_cbus) :
                id(new_id),
                collt(new_collt),
                wlns(new_wlns),
                cbus(new_cbus)
        {}

        void operator()(const coll_report& cr)
        {
                bool must_hit_other = cr.collc != coll_class::projectile;
                bool other_is_enemy = cr.collt != collt;

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
void pain_impl(long id,
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
