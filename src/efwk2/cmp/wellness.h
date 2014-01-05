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

#ifndef WELLNESS_H
#define WELLNESS_H

#include <limits>

#include "../tmp/sfinae.h"

namespace efwk
{

class wellness_regular
{
        double m_health;
        int m_explosions;
        bool m_hurt_flag; // Hackish...

public:
        wellness_regular(double health, int explosions) :
                m_health(health),
                m_explosions(explosions),
                m_hurt_flag(false)
        {}

        void hurt(double d_health)
        {
                m_health -= d_health;
                if (d_health > 0)
                        m_hurt_flag = true;
        }

        void reset_hurt_flag()
        {
                m_hurt_flag = false;
        }

        bool get_hurt_flag() const
        {
                return m_hurt_flag;
        }

        bool alive() const
        {
                return m_health > 0;
        }

        double get_health() const
        {
                return m_health;
        }

        int get_explosions() const
        {
                return m_explosions;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasWellnessRegular, wellness_regular, wlns);

struct wellness_invulnerable
{
        void hurt(double) {}
        void reset_hurt_flag() {}
        bool get_hurt_flag() const { return false; }
        bool alive() const { return true; }

        double get_health() const
        {
                return std::numeric_limits<double>::infinity();
        }

        int get_explosions() const { return 0; }
};

SFINAE__DECLARE_HAS_MEMBER(HasWellnessInvulnerable, wellness_invulnerable, wlns);

template <class T>
using HasWellness = TmpAny<HasWellnessRegular<T>, HasWellnessInvulnerable<T>>;

}

#endif
