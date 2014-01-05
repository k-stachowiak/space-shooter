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
#include "../efwk2/cmp/fx.h"

namespace gplay
{

struct projectile
{
        long id;
        long score_id;
        const char* type_id;
        efwk::appearance_static_bmp appr;
        efwk::life_bounds lbnd;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_circle shp;
        efwk::coll_team collt;
        efwk::coll_class collc;
        efwk::coll_dmg colld;
        efwk::coll_queue collq;
        efwk::wellness_invulnerable wlns;
        efwk::fx_emit_compound eff;

        projectile(long new_id,
                   long new_score_id,
                   ALLEGRO_BITMAP* bmp,
                   double velocity, double dx, double dy,
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
                dyn(0, 0),
                ori(x, y, phi),
                shp(radius),
                collt(is_enemy ? efwk::coll_team::enemy : efwk::coll_team::player),
                collc(efwk::coll_class::projectile),
                colld(damage),
                eff((spark_interval > 0) ? efwk::fx_state::enabled : efwk::fx_state::disabled,
                        spark_interval,
                    (smoke_interval > 0) ? efwk::fx_state::enabled : efwk::fx_state::disabled,
                        smoke_interval)
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
