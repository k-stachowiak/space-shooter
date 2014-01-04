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

#ifndef UCMP_H
#define UCMP_H

#include "../../misc/rand.h"

namespace efwk
{

class cooldown_stat
{
        double m_interval;
        double m_counter;

public:
        cooldown_stat(double interval) :
                m_interval(interval),
                m_counter(0.0)
        {
        }

        bool trigger()
        {
                if (m_counter <= 0.0) {
                        m_counter += m_interval;
                        return true;
                }

                return false;
        }

        void update(double dt)
        {
                if (m_counter > 0.0)
                        m_counter -= dt;
        }
};

class cooldown_rnd
{
        std::uniform_real_distribution<double> m_dist;
        double m_counter;

public:
        cooldown_rnd(double min_interval,
                     double max_interval) :
                m_dist(min_interval, max_interval),
                m_counter(0)
        {}

        bool trigger()
        {
                if (m_counter <= 0) {
                        m_counter = m_dist(rnd::engine);
                        return true;
                }

                return false;
        }

        void update(double dt)
        {
                if (m_counter > 0)
                        m_counter -= dt;
        }
};

}

#endif
