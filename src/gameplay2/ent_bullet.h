/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#ifndef ENT_BULLET_H
#define ENT_BULLET_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/bounds.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"
#include "../efwk2/cmp/shape.h"
#include "../efwk2/cmp/coll_queue.h"

namespace gplay
{

struct bullet
{
        long id;
        const char* type_id;
        efwk::appearance appr;
        efwk::life_bounds lbnd;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_circle shp;
        efwk::coll_queue collq;

        bullet(long new_id,
               ALLEGRO_BITMAP* bmp,
               double velocity, double dx, double dy,
               double x, double y, double phi,
               double x_min, double y_min,
               double x_max, double y_max,
               double radius) :
                id(new_id),
                type_id("bullet"),
                appr(bmp),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(0, 0),
                ori(x, y, phi),
                shp(radius)
        {
                const double dx2 = dx * dx;
                const double dy2 = dy * dy;
                const double len = sqrt(dx2 + dy2);

                if (len != 0) {
                        dyn.vx = dx * velocity / len;
                        dyn.vy = dy * velocity / len;
                }
        }
};

}

#endif
