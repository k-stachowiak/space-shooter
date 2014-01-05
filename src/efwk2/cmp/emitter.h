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

#ifndef EMITTER_H
#define EMITTER_H

#include "common.h"
#include "ucmp.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

namespace efwk
{

struct emitter_spark
{
        cmp_state state;
        cooldown_stat cdown;
        emitter_spark(cmp_state new_state, double interval) :
                state(new_state),
                cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasEmitterSpark, emitter_spark, emit);

struct emitter_smoke
{
        cmp_state state;
        cooldown_stat cdown;
        emitter_smoke(cmp_state new_state, double interval) :
                state(new_state),
                cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasEmitterSmoke, emitter_smoke, emit);

struct emitter_compound
{
        emitter_spark spark;
        emitter_smoke smoke;

        emitter_compound(cmp_state spark_state, double spark_interval,
                         cmp_state smoke_state, double smoke_interval) :
                spark(spark_state, spark_interval),
                smoke(smoke_state, smoke_interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasEmitterCompound, emitter_compound, emit);

template <class T>
using HasEmitter = TmpAny<HasEmitterSmoke<T>,
                          HasEmitterSpark<T>,
                          HasEmitterCompound<T>>;

}

#endif
