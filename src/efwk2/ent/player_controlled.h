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

#ifndef PLAYER_CONTROLLED_H
#define PLAYER_CONTROLLED_H

#include "../cmp/appearance.h"
#include "../cmp/bounds.h"
#include "../cmp/dynamics.h"
#include "../cmp/orientation.h"
#include "../cmp/weaponry.h"

namespace efwk
{

struct player_controlled
{
        appearance appr;
        player_ctrl_dynamics dyn;
        orientation ori;
        move_bounds bnd;
        player_weapons pweap;

        player_controlled(const std::string& img_name,
                double velocity, const std::map<int, bool>& keys,
                double x, double y, double phi,
                double x_min, double y_min, double x_max, double y_max,
                double minigun_interval, double rocket_launcher_interval) :
                        appr(nullptr),
                        dyn(velocity, keys),
                        ori(x, y, phi),
                        bnd(x_min, y_min, x_max, y_max),
                        pweap(minigun_interval, rocket_launcher_interval)
        {
                appr.current_bitmap = al_load_bitmap(img_name.c_str());
        }

        ~player_controlled()
        {
                al_destroy_bitmap(appr.current_bitmap);
        }
};

}

#endif
