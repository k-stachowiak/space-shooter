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

#include <cmath>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "../../misc/config.h"
#include "dynamics.h"

namespace cmp {

class complex_dynamics : public dynamics {
        std::vector<std::shared_ptr<dynamics>> _ds;
public:
        complex_dynamics(std::vector<std::shared_ptr<dynamics>> ds)
        : _ds(ds) {}

        void update(double dt) {
                _vx = _vy = _theta = 0.0;
                for(auto const& d : _ds) {
                        d->update(dt);
                        _vx += d->get_vx();
                        _vy += d->get_vy();
                        _theta += d->get_theta();
                }
        }

        void input(std::map<int, bool>& keys) {
                for(auto const& d : _ds) {
                        d->input(keys);
                }
        }
};

std::shared_ptr<dynamics> create_complex_dynamics(std::vector<std::shared_ptr<dynamics>> ds) {
        return std::shared_ptr<dynamics>(new complex_dynamics(ds));
}

class const_velocity_dynamics : public dynamics {
public:
        const_velocity_dynamics(double vx, double vy) {
               _vx = vx;
               _vy = vy;
        }

        void update(double) {}
        void input(std::map<int, bool>& keys) {}
};

std::shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy) {
        return std::shared_ptr<dynamics>(new const_velocity_dynamics(vx, vy));
}

class const_acc_dynamics : public dynamics {
        double _vx0, _vy0;
        double _ax, _ay;
        double _t;
public:
        const_acc_dynamics(double vx0, double vy0, double ax, double ay)
        : _vx0(vx0), _vy0(vy0), _ax(ax), _ay(ay), _t(0)
        {}

        void update(double dt) {
                _t += dt;
                _vx = _vx0 + _ax * _t;
                _vy = _vy0 + _ay * _t;
        }

        void input(std::map<int, bool>& keys) {}
};

std::shared_ptr<dynamics> create_const_acc_dynamics(double vx0, double vy0, double ax, double ay) {
        return std::shared_ptr<dynamics>(new const_acc_dynamics(vx0, vy0, ax, ay));
}

class const_ang_vel_dynamics : public dynamics {
public:
        const_ang_vel_dynamics(double theta) { _theta = theta; }
        void update(double) {}
        void input(std::map<int, bool>& keys) {}
};

std::shared_ptr<dynamics> create_const_ang_vel_dynamics(double theta) {
        return std::shared_ptr<dynamics>(new const_ang_vel_dynamics(theta));
}

class player_controlled_dynamics : public dynamics {
        double _throttle_x;
        double _throttle_y;
public:
        player_controlled_dynamics()
        : _throttle_x(0.0)
        , _throttle_y(0.0)
        {}

        void update(double) {
                _vx = _throttle_x * cfg::real("gameplay_player_vx");
                _vy = _throttle_y * cfg::real("gameplay_player_vy");
        }

        void input(std::map<int, bool>& keys) {
                _throttle_x = 0.0;
            _throttle_y = 0.0;
                if(keys[ALLEGRO_KEY_RIGHT]) _throttle_x += 1.0;
                if(keys[ALLEGRO_KEY_LEFT]) _throttle_x -= 1.0;
                if(keys[ALLEGRO_KEY_DOWN]) _throttle_y += 1.0;
                if(keys[ALLEGRO_KEY_UP]) _throttle_y -= 1.0;
        }
};

std::shared_ptr<dynamics> create_player_controlled_dynamics() {
        return std::shared_ptr<dynamics>(new player_controlled_dynamics);
}

class path_dynamics : public dynamics {
        // Configuration.
        const std::vector<point> _points;
        const double _lin_vel;

        // State.
        double _x, _y;
        uint32_t _next_point;
        bool _done;

public:
        path_dynamics(std::vector<point> points, double lin_vel)
        : _points(points)
        , _lin_vel(lin_vel)
        , _x(points.front().x)
        , _y(points.front().y)
        , _next_point(1)
        , _done(false)
        {}

        void update(double dt) {

                if(_done)
                        return;

                // Compute the translation towards next point.
                // -------------------------------------------

                // The direction derived from the current segment.
                double dir_x = _points[_next_point].x - _points[_next_point - 1].x;
                double dir_y = _points[_next_point].y - _points[_next_point - 1].y;
                double rsqrt = 1.0 / sqrt(dir_x * dir_x + dir_y * dir_y);

                // Compute the result velocity.
                _vx = dir_x * rsqrt * _lin_vel;
                _vy = dir_y * rsqrt * _lin_vel;

                // Compute the translation for the given dt and analyze it.
                double dx = _vx * dt;
                double dy = _vy * dt;
                double new_x = _x + dx;
                double new_y = _y + dy;

                // Check if the next point has been stepped over.
                // ----------------------------------------------

                // Determine the vectors:
                // - towards the next point (n),
                // - towards the new position (p).
                double ndx = _points[_next_point].x - _x;
                double ndy = _points[_next_point].y - _y;
                double pdx = dx;
                double pdy = dy;

                // Detect moving on to another path's segment.
                // -------------------------------------------
                double dot = (ndx * pdx) + (ndy * pdy);
                if(dot < 0.0) {
                        ++_next_point;
                        if(_next_point >= _points.size())
                                _done = true;
                }

                // Store the new position.
                _x = new_x;
                _y = new_y;
        }

        void input(std::map<int, bool>& keys) {}

        void debug_draw() const {
                for(unsigned i = 1; i < _points.size(); ++i) {
                        double x1 = _points[i - 1].x;
                        double y1 = _points[i - 1].y;
                        double x2 = _points[i - 0].x;
                        double y2 = _points[i - 0].y;
                        al_draw_line(x1, y1, x2, y2, al_map_rgb_f(0,1,1), 2);
                }
        }
};

std::shared_ptr<dynamics> create_path_dynamics(std::vector<point> points, double lin_vel) {
        return std::shared_ptr<dynamics>(new path_dynamics(points, lin_vel));
}

}
