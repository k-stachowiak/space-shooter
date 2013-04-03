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

#include <random>
using std::uniform_real_distribution;
using std::uniform_int_distribution;

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "../misc/config.h"
#include "../misc/rand.h"
#include "../geometry/misc.h"

#include "cmp_shape.h"

namespace cmp {

// Collision declarations.
// -----------------------

bool collide_circle_circle(const circle& a, const circle& b);

// Shape classes.
// --------------

class circle : public shape {
        double _x;
        double _y;
        double _r;
public:
        circle(double x, double y, double r)
        : _x(x), _y(y), _r(r) {}

        double get_x() const { return _x; }
        double get_y() const { return _y; }
        double get_r() const { return _r; }

        void shift(double dx, double dy) { _x += dx; _y += dy; }

        void rotate(double dphi) {}

        bool collides_with(const shape& s) const {
                return s.collides_with_circle(*this);
        }

        bool collides_with_circle(const circle& c) const {
                return collide_circle_circle(*this, c);
        }

        pair<double, double> get_random_point() const {
                uniform_real_distribution<double> dist;
                double t = cfg::math::two_pi * dist(rnd::engine);
                double u = dist(rnd::engine) + dist(rnd::engine);
                double r = (u > 1.0) ? (2.0 - u) : u;
                return make_pair(_x + _r * r * cos(t),
                                 _y + _r * r * sin(t));
        }

        void debug_draw() {
                al_draw_circle(
                                _x, _y, _r,
                                al_map_rgb_f(cfg::real("debug_shape_r"),
                                                         cfg::real("debug_shape_g"),
                                                         cfg::real("debug_shape_b")),
                                1.0);
        }
};

shared_ptr<shape> create_circle(double x, double y, double r) {
        return shared_ptr<shape>(new circle(x, y, r));
}

class complex_shape : public shape {
        vector<shared_ptr<shape>> _shapes;
public:
        complex_shape(vector<shared_ptr<shape>> shapes) : _shapes(shapes) {}

        void shift(double dx, double dy) {
                for(auto& s : _shapes) {
                        s->shift(dx, dy);
                }
        }

        void rotate(double) {
                // Rotation not implemented.
                // Should more sophisticated geometry be applied,
                // use a 3rd party library...
        }

        bool collides_with(shape const& shp) const {
                for(auto& s : _shapes) {
                        if(s->collides_with(shp)) {
                                return true;
                        }
                }
                return false;
        }

        bool collides_with_circle(circle const& c) const {
                for(auto& s : _shapes) {
                        if(s->collides_with_circle(c)) {
                                return true;
                        }
                }
                return false;
        }

        pair<double, double> get_random_point() const {
                uniform_int_distribution<size_t> dist(0, _shapes.size() - 1);
                return _shapes.at(dist(rnd::engine))->get_random_point();
        }

        void debug_draw() {
                for(auto& s : _shapes) {
                        s->debug_draw();
                }
        }
};

shared_ptr<shape> create_complex_shape(vector<shared_ptr<shape>> shapes) {
        return shared_ptr<shape>(new complex_shape(shapes));
}

// Collision implementations.
// --------------------------

bool collide_circle_circle(const circle& a, const circle& b) {
        double dx = b.get_x() - a.get_x();
        double dy = b.get_y() - a.get_y();
        double length = 1.0 / Q_rsqrt(dx * dx + dy * dy);
        return length < (a.get_r() + b.get_r());
}

}
