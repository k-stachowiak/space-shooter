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

#include "../tmp/sfinae.h"

namespace efwk
{

class cooldown
{
        double m_interval;
        double m_counter;

public:
        cooldown(double interval) :
                m_interval(interval),
                m_counter(0.0)
        {
        }

        bool trigger()
        {
                if(m_counter <= 0.0) {
                        m_counter += m_interval;
                        return true;
                }

                return false;
        }

        void update(double dt)
        {
                if(m_counter > 0.0)
                        m_counter -= dt;
        }
};

class player_weapons
{
        cooldown m_minigun;
        cooldown m_rocket_launcher;

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

}

#endif
