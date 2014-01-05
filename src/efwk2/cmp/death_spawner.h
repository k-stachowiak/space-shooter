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

#ifndef DEATH_SPAWNER_H
#define DEATH_SPAWNER_H

#include "../tmp/sfinae.h"

namespace efwk
{

struct death_spawner
{
        int num_sparks;
        int num_explosions;

        death_spawner(int new_num_sparks,
                      int new_num_explosions) :
                num_sparks(new_num_sparks),
                num_explosions(new_num_explosions)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasDeathSpawner, death_spawner, dspwn);

}

#endif
