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

#include <memory>

namespace cmp {

    // Universal type defining an AABB box.
    class bounds {
            double _x_min;
            double _y_min;
            double _x_max;
            double _y_max;
    public:
            bounds(double x_min, double y_min, double x_max, double y_max)
            : _x_min(x_min), _y_min(y_min), _x_max(x_max), _y_max(y_max)
            {}

            double get_x_min() const { return _x_min; }
            double get_y_min() const { return _y_min; }
            double get_x_max() const { return _x_max; }
            double get_y_max() const { return _y_max; }
    };
    
    std::shared_ptr<bounds> create_bounds(double x_min, double y_min, double x_max, double y_max);

}

#endif
