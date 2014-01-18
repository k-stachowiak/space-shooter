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

#ifndef UPGRADE_H
#define UPGRADE_H

namespace efwk
{

class upgrade
{
        const double m_max_gun;
        const double m_max_missile;
        double m_gun;
        double m_missile;

public:
        upgrade(double max_gun, double max_missile) :
                m_max_gun(max_gun),
                m_max_missile(max_missile),
                m_gun(0),
                m_missile(0)
        {}

        void upgrade_gun()
        {
                m_gun += 1.0;
                if (m_gun >= m_max_gun)
                        m_gun = m_max_gun;
        }

        void upgrade_missile()
        {
                m_missile += 1.0;
                if (m_missile >= m_max_missile)
                        m_missile = m_max_missile;
        }

        void wear(double gun_drain, double missile_drain)
        {
                m_gun -= gun_drain;
                m_missile -= missile_drain;
        }

        int get_gun_level() const
        {
                return m_gun;
        }

        int get_missile_level() const
        {
                return m_missile;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasUpgrade, upgrade, upgr);

}

#endif
