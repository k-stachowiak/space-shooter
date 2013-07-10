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

#include <memory>
#include "upgrades.h"

namespace cmp {

    std::shared_ptr<upgrades> create_upgrades(
                    unsigned gun_lvl_max,
                    unsigned rl_lvl_max,
                    unsigned gun_upgrade_ammo,
                    unsigned rl_upgrade_ammo) {
            return std::make_shared<upgrades>(
                            gun_lvl_max,
                            rl_lvl_max,
                            gun_upgrade_ammo,
                            rl_upgrade_ammo);
    }

}

