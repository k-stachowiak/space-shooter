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

#include <iostream>
#include <sstream>

#include <random>
using std::uniform_real_distribution;
using std::uniform_int_distribution;

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "../../misc/config.h"
#include "../../misc/rand.h"

#include "shape.h"

namespace cmp {

// Collision declarations.
// -----------------------

bool collide_circle_circle(const circle& a, double xa, double ya,
                           const circle& b, double xb, double yb);

// Shape classes.
// --------------

class circle : public shape {
        double _r;
public:
        circle(double r)
        : _r(r) {}

        double get_r() const { return _r; }

        bool collides_with(double xa, double ya,
                           const shape& s, double xb, double yb) const {
                return s.collides_with_circle(xa, ya, *this, xb, yb);
        }

        bool collides_with_circle(double xa, double ya,
                                  const circle& c, double xb, double yb) const {
                return collide_circle_circle(*this, xa, ya, c, xb, yb);
        }

        pair<double, double> get_random_point() const {
                uniform_real_distribution<double> dist;
                double t = cfg::math::two_pi * dist(rnd::engine);
                double u = dist(rnd::engine) + dist(rnd::engine);
                double r = (u > 1.0) ? (2.0 - u) : u;
                return make_pair(_r * r * cos(t), _r * r * sin(t));
        }

        void debug_draw(double x, double y) const {
                al_draw_circle(x, y, _r,
                               al_map_rgb_f(cfg::real("debug_shape_r"),
                                            cfg::real("debug_shape_g"),
                                            cfg::real("debug_shape_b")),
                               1.0);
        }

        std::string debug_str() const {
                std::stringstream ss;
                ss << "(" << _r << ")";
                return ss.str();
        }
};

shared_ptr<shape> create_circle(double r) {
        return shared_ptr<shape>(new circle(r));
}

class complex_shape : public shape {
        vector<pair<shared_ptr<shape>, offset>> _shapes;
public:
        complex_shape(vector<pair<shared_ptr<shape>, offset>> shapes)
        : _shapes(shapes) {}

        bool collides_with(double xa, double ya,
                           shape const& shp, double xb, double yb) const {

                for(auto& s : _shapes) {
                        double xa_off = xa + s.second.dx;
                        double ya_off = ya + s.second.dy;
                        if(s.first->collides_with(xa_off, ya_off, shp, xb, yb)) {
                                return true;
                        }
                }
                return false;
        }

        bool collides_with_circle(double xa, double ya,
                                  circle const& c, double xb, double yb) const {
                for(auto& s : _shapes) {
                        double xa_off = xa + s.second.dx;
                        double ya_off = ya + s.second.dy;
                        if(s.first->collides_with_circle(xa_off, ya_off, c, xb, yb)) {
                                return true;
                        }
                }
                return false;
        }

        pair<double, double> get_random_point() const {
                
                uniform_int_distribution<unsigned> dist(0, _shapes.size() - 1);
                unsigned index = dist(rnd::engine);

                auto shp = _shapes.at(index).first;
                auto off = _shapes.at(index).second;

                auto result = shp->get_random_point();

                return { result.first + off.dx, result.second + off.dy };
        }

        void debug_draw(double x, double y) const {
                for(auto& s : _shapes) {
                        double x_off = x + s.second.dx;
                        double y_off = y + s.second.dy;
                        s.first->debug_draw(x_off, y_off);
                }
        }

        std::string debug_str() const {
                std::stringstream ss;
                ss << "cplx{";
                for(auto& s : _shapes) ss << s.first->debug_str() << " ";
                ss << "}";                
                return ss.str();
        }
};

shared_ptr<shape> create_complex_shape(
                vector<pair<shared_ptr<shape>, offset>> shapes) {
        return shared_ptr<shape>(new complex_shape(shapes));
}

// Collision implementations.
// --------------------------

bool collide_circle_circle(const circle& a, double xa, double ya,
                           const circle& b, double xb, double yb) {

        double dx = xb - xa;
        double dy = yb - ya;
        double length = sqrt(dx * dx + dy * dy);
        return length < (a.get_r() + b.get_r());
}

}
