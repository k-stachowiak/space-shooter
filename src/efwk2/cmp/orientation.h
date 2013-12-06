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

#ifndef ORIENTATION_H
#define ORIENTATION_H

namespace efwk
{

class orientation
{
        double m_x;
        double m_prev_x;

        double m_y;
        double m_prev_y;

        double m_phi;
        double m_prev_phi;

        void m_shift_x(double dx)
        {
                m_prev_x = m_x;
                m_x += dx;
        }

        void m_shift_y(double dy)
        {
                m_prev_y = m_y;
                m_y += dy;
        }

public:
        orientation(double x, double y, double phi) :
                        m_x(x), m_prev_x(x),
                        m_y(y), m_prev_y(y),
                        m_phi(phi), m_prev_phi(phi)
        {
        }

        void shift(double dx, double dy)
        {
                m_shift_x(dx);
                m_shift_y(dy);
        }

        void rot(double theta)
        {
                m_prev_phi = m_phi;
                m_phi += theta;
        }

        std::pair<double, double> interpolate_loc(double weight) const
        {
                return std::make_pair(
                        m_x * weight + m_prev_x * (1.0 - weight),
                        m_y * weight + m_prev_y * (1.0 - weight));
        }

        double interpolate_rot(double weight) const
        {
                return m_phi * weight + m_prev_phi * (1.0 - weight);
        }

        void cancel_move()
        {
                m_x = m_prev_x;
                m_y = m_prev_y;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasOrientation, orientation, ori);

}

#endif
