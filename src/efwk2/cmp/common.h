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

#ifndef COMMON_H
#define COMMON_H

namespace efwk
{

enum class cmp_state { disabled, enabled };

enum class bp_state { first, second };

struct point { double x, y; };

struct bounds
{
        double x_min;
        double y_min;
        double x_max;
        double y_max;

protected:
        bounds(double new_x_min, double new_y_min,
               double new_x_max, double new_y_max) :
                x_min(new_x_min), y_min(new_y_min),
                x_max(new_x_max), y_max(new_y_max)
        {
        }

        friend bool point_in_x_bounds(double x, double y, const bounds& bnd)
        {
                return x >= bnd.x_min && x <= bnd.x_max;
        }

        friend bool point_in_y_bounds(double x, double y, const bounds& bnd)
        {
                return y >= bnd.y_min && y <= bnd.y_max;
        }

        friend bool point_in_bounds(double x, double y, const bounds& bnd)
        {
                return point_in_x_bounds(x, y, bnd) &&
                       point_in_y_bounds(x, y, bnd);
        }

};

}

#endif
