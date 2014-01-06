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

#ifndef PAIN_SPAWNER_H
#define PAIN_SPAWNER_H

#include "common.h"
#include "ucmp.h"
#include "../tmp/sfinae.h"

namespace efwk
{

struct pain_spawner
{
        cmp_state smoke_state;
        double smoke_threshold; // i.e. the health ratio.
        cooldown_stat smoke_cdown;

        pain_spawner(cmp_state new_smoke_state,
                     double smoke_threshold,
                     double smoke_interval) :
                smoke_state(new_smoke_state),
                smoke_threshold(smoke_threshold),
                smoke_cdown(smoke_interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasPainSpawner, pain_spawner, pspwn);

}

#endif
