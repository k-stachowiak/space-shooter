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
#include "../efwk2/cmp/coll_traits.h"
#include "../efwk2/cmp/wellness.h"
#include "../efwk2/cmp/flash.h"
#include "../efwk2/cmp/death_spawner.h"
#include "../efwk2/cmp/pain_spawner.h"

namespace gplay
{

struct enemy
{
        long id;
        long score_id;
        const char* type_id;
        efwk::flash flsh;
        efwk::appearance_bin_proxy<
                efwk::appearance_static_bmp,
                efwk::appearance_static_bmp> appr;
        efwk::life_bounds lbnd;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;
        efwk::shape_bin_proxy<
                efwk::shape_circle,
                efwk::shape_compound<
                        efwk::shape_circle,
                        efwk::shape_circle,
                        efwk::shape_circle>> shp;
        efwk::coll_traits ctraits;
        efwk::wellness_regular wlns;
        efwk::death_spawner dspwn;
        efwk::pain_spawner pspwn;

        enemy(const long new_id,
              ALLEGRO_BITMAP* const bmp,
              ALLEGRO_BITMAP* const bmp_flash,
              const double velocity,
              const double x, const double y,
              const double x_min, const double y_min,
              const double x_max, const double y_max,
              const double radius0,
              const double radius1, const double radius2, const double radius3,
              const double x1, const double y1,
              const double x2, const double y2,
              const double x3, const double y3,
              const bool is_single_circle,
              const double damage,
              const double health,
              const int explosions,
              const double health_dist,
              const double pain_smoke_threshold,
              const double pain_smoke_interval) :
                id(new_id),
                score_id(new_id),
                type_id("enemy"),
                flsh(0.05),
                appr(bmp, bmp_flash),
                lbnd(x_min, y_min, x_max, y_max),
                dyn(0, velocity),
                ori(x, y, 3.1415 * 0.5),
                shp(efwk::shape_circle { radius0 },
                    efwk::shape_compound<efwk::shape_circle, efwk::shape_circle, efwk::shape_circle> {
                        std::make_pair(efwk::shape_circle { radius1 },
                                       efwk::orientation { x1, y1, 0.0 }),
                        std::make_pair(efwk::shape_circle { radius2 },
                                       efwk::orientation { x2, y2, 0.0 }),
                        std::make_pair(efwk::shape_circle { radius3 },
                                       efwk::orientation { x3, y3, 0.0 }) },
                    is_single_circle ? efwk::bp_state::first : efwk::bp_state::second),
                ctraits(efwk::coll_team::enemy,
                        efwk::coll_class::ship,
                        damage,
                        0),
                wlns(health),
                dspwn(0, explosions, health_dist),
                pspwn(efwk::cmp_state::enabled,
                      pain_smoke_threshold,
                      pain_smoke_interval)
        {
        }
};

}

#endif
