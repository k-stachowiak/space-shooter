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

#ifndef ENT_PICKUP_H
#define ENT_PICKUP_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/bounds.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"
#include "../efwk2/cmp/shape.h"
#include "../efwk2/cmp/coll_traits.h"
#include "../efwk2/cmp/coll_queue.h"

namespace gplay
{

struct pickup
{
        long id;
        long score_id;
        const char* type_id;
        efwk::appearance_static_bmp appr;
        efwk::life_bounds lbnd;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_circle shp;
        efwk::coll_traits ctraits;
        efwk::coll_queue cqueue;

        pickup(const long new_id,
               const long new_score_id,
               ALLEGRO_BITMAP* const bmp,
               const double vx, const double vy,
               const double x, const double y,
               const double x_min, const double y_min,
               const double x_max, const double y_max,
               const double radius,
               const double pick_health,
               const bool pick_bullupgr,
               const bool pick_missupgr) :
                id(new_id),
                score_id(new_score_id),
                type_id("pickup"),
                appr(bmp),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(vx, vy),
                ori(x, y, 0),
                shp(radius),
                ctraits(efwk::coll_team::none,
                        efwk::coll_class::pickup,
                        0,
                        pick_health,
                        pick_bullupgr,
                        pick_missupgr)
        {
        }
};

}

#endif
