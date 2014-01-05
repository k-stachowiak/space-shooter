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

#ifndef ENT_PROJECTILE_H
#define ENT_PROJECTILE_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/bounds.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"
#include "../efwk2/cmp/shape.h"
#include "../efwk2/cmp/coll_traits.h"
#include "../efwk2/cmp/wellness.h"
#include "../efwk2/cmp/emitter.h"

namespace gplay
{

struct projectile
{
        long id;
        long score_id;
        const char* type_id;
        efwk::appearance_static_bmp appr;
        efwk::life_bounds lbnd;
        efwk::const_acc_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_circle shp;
        efwk::coll_traits ctraits;
        efwk::wellness_invulnerable wlns;
        efwk::emitter_compound emit;

        projectile(long new_id,
                   long new_score_id,
                   ALLEGRO_BITMAP* bmp,
                   double velocity, double dx, double dy,
                   double acceleration,
                   double x, double y, double phi,
                   double x_min, double y_min,
                   double x_max, double y_max,
                   double radius,
                   bool is_enemy, double damage,
                   double spark_interval,
                   double smoke_interval) :
                id(new_id),
                score_id(new_score_id),
                type_id("projectile"),
                appr(bmp),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(0, 0, 0, 0), // Decided to initialize this in the body.
                ori(x, y, phi),
                shp(radius),
                ctraits(is_enemy ? efwk::coll_team::enemy : efwk::coll_team::player,
                        efwk::coll_class::projectile,
                        damage),
                emit((spark_interval > 0)
                        ? efwk::cmp_state::enabled
                        : efwk::cmp_state::disabled,
                      spark_interval,
                     (smoke_interval > 0)
                        ? efwk::cmp_state::enabled
                        : efwk::cmp_state::disabled,
                      smoke_interval)
        {
                // Didn't want to force feed this into the initialization list,
                // but it can be done for the sake of constness and encapsulation.
                // If you read this, the above values have not turned out to be
                // imortant enough so far.
                const double dx2 = dx * dx;
                const double dy2 = dy * dy;
                const double len = sqrt(dx2 + dy2);
                if (len != 0) {
                        dyn.m_vx0 = dx * velocity / len;
                        dyn.m_vy0 = dy * velocity / len;
                }

                double mul = is_enemy ? 1 : -1;
                dyn.m_ay = mul * acceleration;
        }
};

}

#endif
