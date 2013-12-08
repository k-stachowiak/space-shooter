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

#ifndef ENEMY_H
#define ENEMY_H

namespace efwk
{

struct enemy
{
        long id;
        appearance appr;
        life_bounds lbnd;
        const_vel_dynamics dyn;
        orientation ori;

        enemy(long new_id,
              ALLEGRO_BITMAP* bmp,
              double velocity,
              double x, double y,
              double x_min, double y_min,
              double x_max, double y_max) :
                id(new_id),
                appr(bmp),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(0, velocity),
                ori(x, y, 3.1415 * 0.5)
        {
        }
};

}

#endif
