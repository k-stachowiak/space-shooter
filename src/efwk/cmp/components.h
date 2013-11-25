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

#ifndef COMPONENTS_H
#define COMPONENTS_H

namespace cmp {

    // TODO: Where to put these?

    // The drawing order management.
    enum class draw_plane {
            BACKGROUND,
            SHIPS,
            FX,
            PROJECTILES
    };

    // Score class id.
    enum class score_class {
            PLAYER,
            ENEMY_LIGHT_FIGHTER,
            ENEMY_HEAVY_FIGHTER,
            ENEMY_LIGHT_BOMBER,
            ENEMY_HEAVY_BOMBER
    };


}

#include "appr.h"
#include "bounds.h"
#include "col_profile.h"
#include "col_report.h"
#include "coll_queue.h"
#include "dynamics.h"
#include "fx.h"
#include "noise_queue.h"
#include "orientation.h"
#include "pickup_profile.h"
#include "reaction.h"
#include "shape.h"
#include "timer.h"
#include "upgrade_atom.h"
#include "upgrades.h"
#include "weapon.h"
#include "weapon_beh.h"
#include "wellness.h"

#endif
