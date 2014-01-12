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
        double m_health_max;
        bool m_hurt_flag; // Hackish...

public:
        wellness_regular(double health) :
                m_health(health),
                m_health_max(health),
                m_hurt_flag(false)
        {}

        void hurt(double d_health)
        {
                // Note that the delta health argument
                // doesn't have to be positive.
                m_health -= d_health;
                if (d_health > 0)
                        m_hurt_flag = true;

                // Cap the health value.
                if (m_health < 0)
                        m_health = 0;
                if (m_health > m_health_max)
                        m_health = m_health_max;
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

        double get_health_max() const
        {
                return m_health_max;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasWellnessRegular, wellness_regular, wlns);

struct wellness_paper
{
        bool is_alive;

        wellness_paper() : is_alive(true) {}

        void hurt(double) { is_alive = false; }
        void reset_hurt_flag() {}
        bool get_hurt_flag() const { return !is_alive; }
        bool alive() const { return is_alive; }
        double get_health() const { return 0; }
};

SFINAE__DECLARE_HAS_MEMBER(HasWellnessPaper, wellness_paper, wlns);

template <class T>
using HasWellness = TmpAny<HasWellnessRegular<T>, HasWellnessPaper<T>>;

}

#endif
