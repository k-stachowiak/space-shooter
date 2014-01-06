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
        efwk::shape_circle shp;
        efwk::coll_traits ctraits;
        efwk::wellness_regular wlns;
        efwk::death_spawner dspwn;
        efwk::pain_spawner pspwn;

        enemy(const long new_id,
              ALLEGRO_BITMAP* const bmp,
              ALLEGRO_BITMAP* const bmp_flash,
              const double velocity,
              const double x,
              const double y,
              const double x_min,
              const double y_min,
              const double x_max,
              const double y_max,
              const double radius,
              const double damage,
              const double health,
              const double explosions,
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
                shp(radius),
                ctraits(efwk::coll_team::enemy,
                        efwk::coll_class::ship,
                        damage),
                wlns(health),
                dspwn(0, 1),
                pspwn(efwk::cmp_state::enabled,
                      pain_smoke_threshold,
                      pain_smoke_interval)
        {
        }
};

}

#endif
