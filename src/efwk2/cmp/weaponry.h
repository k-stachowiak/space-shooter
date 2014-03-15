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

#ifndef WEAPONRY_H
#define WEAPONRY_H

#include "ucmp.h"
#include "../tmp/sfinae.h"

namespace efwk
{

class player_weapons
{
        cooldown_stat m_minigun;
        cooldown_stat m_rocket_launcher;

public:
        player_weapons(double primary_interval,
                       double secondary_interval) :
                m_minigun(primary_interval),
                m_rocket_launcher(secondary_interval)
        {
        }

        void update(double dt)
        {
                m_minigun.update(dt);
                m_rocket_launcher.update(dt);
        }

        bool trigger_primary()
        {
                return m_minigun.trigger();
        }

        bool trigger_secondary()
        {
                return m_rocket_launcher.trigger();
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasPlayerWeapons, player_weapons, pweap);

struct weap_period_bullet
{
        cooldown_stat cdown;
        double x_offset;
        double y_offset;
        double y_dir;
};

SFINAE__DECLARE_HAS_MEMBER(HasPeriodBullet, weap_period_bullet, weap);

struct weap_period_missile
{
        cooldown_stat cdown;
        double x_offset;
        double y_offset;
        double y_dir;
};

SFINAE__DECLARE_HAS_MEMBER(HasPeriodMissile, weap_period_missile, weap);

struct weap_compound_base {};

template <class... Weaps>
struct weap_compound : public weap_compound_base
{
        std::tuple<Weaps...> impl;
        weap_compound(const Weaps&... new_impl) :
                impl(new_impl...)
        {}
};

template <class Func, int Index, class... Weaps>
struct cpd_weap_functor
{
        void operator()(const weap_compound<Weaps...>& cpd, Func func)
        {
                func(Index, std::get<Index>(cpd.impl));
                cpd_weap_functor<Func, Index - 1, Weaps...> ftor;
                ftor(cpd, func);
        }
};

template <class Func, class... Weaps>
struct cpd_weap_functor<Func, -1, Weaps...>
{
        void operator()(const weap_compound<Weaps...>&, Func) {}
};

template <class Func, class... Weaps>
void for_each_weap(const weap_compound<Weaps...>& cpd, Func func)
{
        cpd_weap_functor<Func, sizeof...(Weaps) - 1, Weaps...> fctor;
        fctor(cpd, func);
}

SFINAE__DECLARE_HAS_MEMBER(HasWeaponCompound, weap_compound_base, weap);

template <class T>
using HasWeapon = TmpAny<HasPeriodBullet<T>,
                         HasPeriodMissile<T>,
                         HasWeaponCompound<T>>;

}

#endif
