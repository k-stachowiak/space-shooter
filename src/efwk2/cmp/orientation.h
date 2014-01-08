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

#include "../tmp/sfinae.h"

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

        void cancel_x_move()
        {
                m_x = m_prev_x;
        }

        void cancel_y_move()
        {
                m_y = m_prev_y;
        }

        void cancel_move()
        {
                cancel_x_move();
                cancel_y_move();
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasOrientation, orientation, ori);

inline
orientation compose(const orientation& ori1, const orientation& ori2)
{
        double phi1 = ori1.interpolate_rot(0);
        double phi2 = ori2.interpolate_rot(0);
        double x1, y1, x2, y2;
        std::tie(x1, y1) = ori1.interpolate_loc(0);
        std::tie(x2, y2) = ori2.interpolate_loc(0);

        double mat1[3][3] = {
                { +cos(phi1), sin(phi1), x1 },
                { -sin(phi1), cos(phi1), y1 },
                { 0,          0,         1  }
        };

        double mat2[3][3] = {
                { +cos(phi2), sin(phi2), x2 },
                { -sin(phi2), cos(phi2), y2 },
                { 0,          0,         1  }
        };

        double mat_mul[3][3] = {
                { mat1[0][0] * mat2[0][0] + mat1[0][1] * mat2[1][0] + mat1[0][2] * mat2[2][0],
                  mat1[0][0] * mat2[0][1] + mat1[0][1] * mat2[1][1] + mat1[0][2] * mat2[2][1],
                  mat1[0][0] * mat2[0][2] + mat1[0][1] * mat2[1][2] + mat1[0][2] * mat2[2][2] },

                { mat1[1][0] * mat2[0][0] + mat1[1][1] * mat2[1][0] + mat1[1][2] * mat2[2][0],
                  mat1[1][0] * mat2[0][1] + mat1[1][1] * mat2[1][1] + mat1[1][2] * mat2[2][1],
                  mat1[1][0] * mat2[0][2] + mat1[1][1] * mat2[1][2] + mat1[1][2] * mat2[2][2] },

                { mat1[2][0] * mat2[0][0] + mat1[2][1] * mat2[1][0] + mat1[2][2] * mat2[2][0],
                  mat1[2][0] * mat2[0][1] + mat1[2][1] * mat2[1][1] + mat1[2][2] * mat2[2][1],
                  mat1[2][0] * mat2[0][2] + mat1[2][1] * mat2[1][2] + mat1[2][2] * mat2[2][2] }
        };

        // TODO: note that not all the computations below are necessary. This can be optimized
        //       should the orientation composition become a bollteleck. Only the indices used
        //       below must be computed.
        double x = mat_mul[0][2];
        double y = mat_mul[1][2];
        double sin_phi = mat_mul[0][1];
        double cos_phi = mat_mul[1][1];
        double phi = atan2(sin_phi, cos_phi);

        return { x, y, phi };
}

}

#endif
