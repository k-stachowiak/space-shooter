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

#ifndef TIME_SPAWNER_H
#define TIME_SPAWNER_H

#include "common.h"
#include "ucmp.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

namespace efwk
{

struct time_spawner_spark
{
        cmp_state state;
        cooldown_stat cdown;
        time_spawner_spark(cmp_state new_state, double interval) :
                           state(new_state),
                           cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasTimeSpawnerSpark, time_spawner_spark, tspwn);

struct time_spawner_smoke
{
        cmp_state state;
        cooldown_stat cdown;
        time_spawner_smoke(cmp_state new_state, double interval) :
                          state(new_state),
                          cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasTimeSpawnerSmoke, time_spawner_smoke, tspwn);

struct time_spawner_compound
{
        time_spawner_spark spark;
        time_spawner_smoke smoke;

        time_spawner_compound(cmp_state spark_state, double spark_interval,
                              cmp_state smoke_state, double smoke_interval) :
                spark(spark_state, spark_interval),
                smoke(smoke_state, smoke_interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasTimeSpawnerCompound, time_spawner_compound, tspwn);

template <class T>
using HasTimeSpawner = TmpAny<HasTimeSpawnerSmoke<T>,
                              HasTimeSpawnerSpark<T>,
                              HasTimeSpawnerCompound<T>>;

}

#endif
