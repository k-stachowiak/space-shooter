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

#ifndef COLL_TRAITS_H
#define COLL_TRAITS_H

#include "../tmp/sfinae.h"

namespace efwk
{

enum class coll_team
{
        player,
        enemy,
        none
};

inline
std::string to_string(const coll_team& ct)
{
        switch (ct) {
        case coll_team::player:
                return "player";
        case coll_team::enemy:
                return "enemy";
        case coll_team::none:
                return "none";
        }

        exit(1);
        return {};
}

enum class coll_class
{
        ship,
        projectile,
        pickup
};

inline
std::string to_string(const coll_class& cc)
{
        switch (cc) {
        case coll_class::ship:
                return "ship";
        case coll_class::projectile:
                return "projectile";
        case coll_class::pickup:
                return "pickup";
        }

        exit(1);
        return {};
}

struct coll_dmg
{
        double damage;
        coll_dmg(double new_damage) : damage(new_damage) {}
};

struct coll_pick
{
        double health;
        bool bullupgr;
        bool missupgr;
        coll_pick(double new_health,
                  bool new_bullupgr,
                  bool new_missupgr) :
                health(new_health),
                bullupgr(new_bullupgr),
                missupgr(new_missupgr)
        {}
};

struct coll_traits
{
        coll_team cteam;
        coll_class cclass;
        coll_dmg cdmg;
        coll_pick cpck;

        coll_traits(const coll_team new_cteam,
                    const coll_class new_cclass,
                    const double damage,
                    const double pick_health,
                    const bool pick_bullupgr,
                    const bool pick_missupgr) :
                cteam(new_cteam),
                cclass(new_cclass),
                cdmg(damage),
                cpck(pick_health, pick_bullupgr, pick_missupgr)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasCollisionTraits, coll_traits, ctraits);

}

#endif
