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

namespace efwk
{

template <class Wellness>
struct ship_reaction_func
{
        long id;
        const coll_team& collt;
        const coll_dmg& colld;
        Wellness& wlns;
        comm_bus& cbus;

        ship_reaction_func(long new_id,
                           const coll_team& new_collt,
                           const coll_dmg& new_colld,
                           Wellness& new_wlns,
                           comm_bus& new_cbus) :
                id(new_id),
                collt(new_collt),
                colld(new_colld),
                wlns(new_wlns),
                cbus(new_cbus)
        {}

        void operator()(const coll_report& cr)
        {
                bool other_hurts =
                        cr.collc == coll_class::ship ||
                        cr.collc == coll_class::projectile;

                bool other_is_enemy = cr.collt != collt;

                if (other_hurts && other_is_enemy) {
                        wlns.hurt(cr.colld.damage);
                }

                if (!wlns.alive()) {
                        cbus.dels.push(id);
                }
        }
};

struct projectile_reaction_func
{
        long id;
        const coll_team& collt;
        comm_bus& cbus;

        projectile_reaction_func(long new_id,
                                 const coll_team& new_collt,
                                 comm_bus& new_cbus) :
                id(new_id),
                collt(new_collt),
                cbus(new_cbus)
        {}

        void operator()(const coll_report& cr)
        {
                bool must_hit_other = cr.collc != coll_class::projectile;
                bool other_is_enemy = cr.collt != collt;

                if (must_hit_other && other_is_enemy) {
                        cbus.dels.push(id);
                }
        }
};

template <class Entity>
void pain_impl(Entity& ent, comm_bus& cbus)
{
        long id = ent.id;
        const coll_class& collc = ent.collc;
        const coll_team& collt = ent.collt;
        const coll_dmg& colld = ent.colld;
        const coll_queue& collq = ent.collq;
        auto& wlns = ent.wlns;

        // TODO: Turn the below into lambdas.
        ship_reaction_func<decltype(ent.wlns)> srf(id, collt, colld, wlns, cbus);
        projectile_reaction_func prf(id, collt, cbus);

        switch (collc) {
        case coll_class::ship:
                collq.for_each_report(srf);
                break;

        case coll_class::projectile:
                collq.for_each_report(prf);
                break;
        }
}

template <class T>
using IsPainable = TmpAll<HasWellness<T>, HasCollisionTraits<T>>;

template <class Entity>
typename std::enable_if<IsPainable<Entity>::value, void>::type
pain(Entity& ent, comm_bus& cbus) { pain_impl(ent, cbus); }

template <class Entity>
typename std::enable_if<!IsPainable<Entity>::value, void>::type
pain(Entity&, comm_bus&) {}

}

#endif
