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

#ifndef UPGRADES_H
#define UPGRADES_H

#include <memory> 

#include "upgrade_atom.h"

namespace cmp {

    class upgrades {
            upgrade_atom _gun;
            upgrade_atom _rl;

    public:
            upgrades(unsigned gun_lvl_max,
                            unsigned rl_lvl_max,
                            unsigned gun_upgrade_ammo,
                            unsigned rl_upgrade_ammo)
            : _gun(1, gun_upgrade_ammo, gun_lvl_max)
            , _rl(1, rl_upgrade_ammo, rl_lvl_max)
            {}

            bool can_upgrade_gun() const { return _gun.can_level_up(); }
            unsigned gun_lvl() const { return _gun.get_level(); }
            unsigned gun_ticks() const { return _gun.get_ticks(); }
            unsigned gun_ticks_per_level() const { return _gun.get_ticks_per_level(); }
            void upgrade_gun() { _gun.level_up(); }
            void tick_down_gun() { _gun.tick_down(); }

            bool can_upgrade_rl() const { return _rl.can_level_up(); }
            unsigned rl_lvl() const { return _rl.get_level(); }
            unsigned rl_ticks() const { return _rl.get_ticks(); }
            unsigned rl_ticks_per_level() const { return _rl.get_ticks_per_level(); }
            void upgrade_rl() { _rl.level_up(); }
            void tick_down_rl() { _rl.tick_down(); }

    };

    std::shared_ptr<upgrades> create_upgrades(
        unsigned gun_lvl_max,
        unsigned rl_lvl_max,
        unsigned gun_upgrade_ammo,
        unsigned rl_upgrade_ammo);

}

#endif
