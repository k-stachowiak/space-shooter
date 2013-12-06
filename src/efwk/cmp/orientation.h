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

#include <utility>
#include <memory>

namespace cmp {

    // The position and the rotation.
    class orientation {
            double _x, _y, _phi;
            double _prev_x, _prev_y, _prev_phi;
    public:
            orientation(double x, double y, double phi)
            : _x(x), _y(y), _phi(phi)
            , _prev_x(x)
            , _prev_y(y)
            , _prev_phi(phi)
            {}

            double get_phi() const { return _phi; }

            void shift_x(double dx) {
                    _prev_x = _x;
                    _x += dx;
            }

            void shift_y(double dy) {
                    _prev_y = _y;
                    _y += dy;
            }

            void shift(double dx, double dy) {
                    shift_x(dx);
                    shift_y(dy);
            }

            void rot(double theta) {
                    _prev_phi = _phi;
                    _phi += theta;
            }

            std::pair<double, double> interpolate_loc(double weight) {
                    return std::make_pair(
                            _x * weight + _prev_x * (1.0 - weight),
                            _y * weight + _prev_y * (1.0 - weight));
            }
    };

    std::shared_ptr<orientation> create_orientation(double x, double y, double theta);

}

#endif
