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

#ifndef DYNAMICS_H
#define DYNAMICS_H

#include <map>
#include <cmath>

#include <allegro5/allegro5.h>

#include "../tmp/map.h"

namespace efwk
{

struct dynamics
{
        double vx;
        double vy;
        double theta;

protected:
        dynamics(double new_vx, double new_vy, double new_theta) :
                vx(new_vx), vy(new_vy), theta(new_theta)
        {
        }
};

class const_vel_dynamics: public dynamics
{
public:
        const_vel_dynamics(double vx, double vy) :
                dynamics(vx, vy, 0)
        {}

        void update(double dt)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasDynamicsConstVel, const_vel_dynamics, dyn);

class player_ctrl_dynamics: public dynamics
{
        const double m_velocity;
        const std::map<int, bool>& m_keys;

public:
        player_ctrl_dynamics(
                double velocity,
                const std::map<int, bool>& keys) :
                        dynamics(0, 0, 0),
                        m_velocity(velocity),
                        m_keys(keys)
        {}

        void update(double dt)
        {
                // Update for input.
                double throttle_x = 0;
                double throttle_y = 0;

                if (m_keys.at(ALLEGRO_KEY_RIGHT))
                        throttle_x += 1.0;

                if (m_keys.at(ALLEGRO_KEY_LEFT))
                        throttle_x -= 1.0;

                if (m_keys.at(ALLEGRO_KEY_DOWN))
                        throttle_y += 1.0;

                if (m_keys.at(ALLEGRO_KEY_UP))
                        throttle_y -= 1.0;

                // Integrate dynamics.
                double dx2 = throttle_x * throttle_x;
                double dy2 = throttle_y * throttle_y;
                double len = sqrt(dx2 + dy2);

                if (throttle_x == 0) {
                        vx = 0;
                } else {
                        vx = (throttle_x / len) * m_velocity;
                }

                if (throttle_y == 0) {
                        vy = 0;
                } else {
                        vy = (throttle_y / len) * m_velocity;
                }
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasDynamicsPlayCtrl, player_ctrl_dynamics, dyn);

template<class T>
using HasDynamics = TmpAny<HasDynamicsConstVel<T>, HasDynamicsPlayCtrl<T>>;

}

#endif /* DYNAMICS_H_ */
