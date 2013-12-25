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

#ifndef BOUNDS_H
#define BOUNDS_H

#include "../tmp/sfinae.h"

namespace efwk
{

struct bounds
{
        double x_min;
        double y_min;
        double x_max;
        double y_max;

        bounds(double new_x_min, double new_y_min,
               double new_x_max, double new_y_max) :
                x_min(new_x_min), y_min(new_y_min),
                x_max(new_x_max), y_max(new_y_max)
        {
        }
};

inline bool point_in_bounds(double x, double y, const bounds& bnd)
{
        return x >= bnd.x_min && x <= bnd.x_max &&
               y >= bnd.y_min && y <= bnd.y_max;
}

struct move_bounds : public bounds
{
        move_bounds(double new_x_min, double new_y_min,
                    double new_x_max, double new_y_max) :
                bounds(new_x_min, new_y_min, new_x_max, new_y_max)
        {
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasMoveBounds, move_bounds, mbnd);

struct life_bounds : public bounds
{
        life_bounds(double new_x_min, double new_y_min,
                    double new_x_max, double new_y_max) :
                bounds(new_x_min, new_y_min, new_x_max, new_y_max)
        {
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasLifeBounds, life_bounds, lbnd);

}

#endif
