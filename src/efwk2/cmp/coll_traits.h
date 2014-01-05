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
        player, enemy
};

inline
std::string to_string(const coll_team& ct)
{
        switch (ct) {
        case coll_team::player:
                return "player";
        case coll_team::enemy:
                return "enemy";
        }

        exit(1);
        return {};
}

enum class coll_class
{
        ship, projectile
};

inline
std::string to_string(const coll_class& cc)
{
        switch (cc) {
        case coll_class::ship:
                return "ship";
        case coll_class::projectile:
                return "projectile";
        }

        exit(1);
        return {};
}

struct coll_dmg
{
        double damage;

        coll_dmg(double new_damage) : damage(new_damage) {}
};

struct coll_report
{
        long id;
        long score_id;
        const char* type_id;
        coll_team collt;
        coll_class collc;
        coll_dmg colld;
        std::vector<point> points;
};

class coll_queue
{
        std::vector<coll_report> m_reports;

public:
        void push(coll_report report)
        {
                m_reports.push_back(std::move(report));
        }

        template<class Func>
        void for_each_report(Func func) const
        {
                std::for_each(begin(m_reports), end(m_reports), func);
        }

        void clear()
        {
                m_reports.clear();
        }
};

// TODO: consider extracting the collision queue as an individual component.
struct coll_traits
{
        coll_team cteam;
        coll_class cclass;
        coll_dmg cdmg;
        coll_queue cqueue;

        coll_traits(coll_team new_cteam,
                    coll_class new_cclass,
                    double damage) :
                cteam(new_cteam),
                cclass(new_cclass),
                cdmg(damage)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasCollisionTraits, coll_traits, ctraits);

}

#endif
