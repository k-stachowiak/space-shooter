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

#ifndef ENT_ENEMY_H
#define ENT_ENEMY_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/bounds.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"
#include "../efwk2/cmp/shape.h"

namespace gplay
{

struct enemy
{
        long id;
        const char* type_id;
        efwk::appearance appr;
        efwk::life_bounds lbnd;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_circle shp;

        enemy(long new_id,
              ALLEGRO_BITMAP* bmp,
              double velocity,
              double x, double y,
              double x_min, double y_min,
              double x_max, double y_max,
              double radius) :
                id(new_id),
                type_id("enemy"),
                appr(bmp),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(0, velocity),
                ori(x, y, 3.1415 * 0.5),
                shp(radius)
        {
        }
};

}

#endif
