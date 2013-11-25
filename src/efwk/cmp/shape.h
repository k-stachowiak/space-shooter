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

#ifndef CMP_SHAPE_H
#define CMP_SHAPE_H

#include <utility>
using std::pair;
using std::make_pair;

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

#include <string>

namespace cmp {

class circle;
class complex_shape;

struct offset { double dx, dy; };

class shape {
public:
        virtual ~shape() {}
        virtual bool collides_with(
                double xa, double ya,
                shape const& shp, double xb, double yb) const = 0;

        virtual bool collides_with_circle(
                double xa, double ya,
                circle const& c, double xb, double yb) const = 0;

        virtual pair<double, double> get_random_point() const = 0;
        virtual void debug_draw(double x, double y) const {}
        virtual std::string debug_str() const { return {}; }
};

shared_ptr<shape> create_circle(double r);
shared_ptr<shape> create_complex_shape(
                vector<pair<shared_ptr<shape>, offset>> shapes);

}


#endif
