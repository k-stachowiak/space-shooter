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
#include "../efwk2/cmp/coll_queue.h"
#include "../efwk2/cmp/wellness.h"
#include "../efwk2/cmp/time_spawner.h"
#include "../efwk2/cmp/death_spawner.h"

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
        efwk::coll_queue cqueue;
        efwk::wellness_paper wlns;
        efwk::time_spawner_compound tspwn;
        efwk::death_spawner dspwn;

        projectile(const long new_id,
                   const long new_score_id,
                   ALLEGRO_BITMAP* const bmp,
                   const double velocity,
                   const double dx,
                   const double dy,
                   const double acceleration,
                   const double x,
                   const double y,
                   const double phi,
                   const double x_min,
                   const double y_min,
                   const double x_max,
                   const double y_max,
                   const double radius,
                   const bool is_enemy,
                   const double damage,
                   const double spark_interval,
                   const double smoke_interval,
                   const int sparks_on_death,
                   const int expl_on_death) :
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
                        damage,
                        0, 0, 0),
                tspwn((spark_interval > 0)
                        ? efwk::cmp_state::enabled
                        : efwk::cmp_state::disabled,
                      spark_interval,
                     (smoke_interval > 0)
                        ? efwk::cmp_state::enabled
                        : efwk::cmp_state::disabled,
                      smoke_interval),
                dspwn(sparks_on_death,
                      expl_on_death,
                      0, 0, 0)
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

                const double mul = is_enemy ? 1 : -1;
                dyn.m_ay = mul * acceleration;
        }
};

}

#endif
