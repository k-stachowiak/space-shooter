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

#ifndef ENT_PLAYER_H
#define ENT_PLAYER_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/bounds.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"
#include "../efwk2/cmp/weaponry.h"
#include "../efwk2/cmp/shape.h"
#include "../efwk2/cmp/coll_traits.h"
#include "../efwk2/cmp/wellness.h"
#include "../efwk2/cmp/flash.h"

namespace gplay 
{

struct player
{
        long id;
        long score_id;
        const char* type_id;
        int score;
        efwk::flash flsh;
        efwk::appearance_bin_proxy<
                efwk::appearance_static_bmp,
                efwk::appearance_static_bmp> appr;
        efwk::player_ctrl_dynamics dyn;
        efwk::orientation ori;
        efwk::move_bounds mbnd;
        efwk::player_weapons pweap;
        efwk::shape_circle shp;
        efwk::coll_traits ctraits;
        efwk::wellness_regular wlns;

        player(long new_id,
               ALLEGRO_BITMAP* const bmp,
               ALLEGRO_BITMAP* const bmp_flash,
               const double velocity,
               const std::map<int, bool>& keys,
               const double x,
               const double y,
               const double phi,
               const double x_min,
               const double y_min,
               const double x_max,
               const double y_max,
               const double minigun_interval,
               const double rocket_launcher_interval,
               const double radius,
               const double damage,
               const double health) :
                        id(new_id),
                        score_id(new_id),
                        type_id("player"),
                        score(0),
                        flsh(0.05),
                        appr(bmp, bmp_flash),
                        dyn(velocity, keys),
                        ori(x, y, phi),
                        mbnd(x_min, y_min, x_max, y_max),
                        pweap(minigun_interval, rocket_launcher_interval),
                        shp(radius),
                        ctraits(efwk::coll_team::player,
                                efwk::coll_class::ship,
                                damage),
                        wlns(health)
        {
        }
};

}

#endif
