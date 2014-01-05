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

#ifndef FX_H
#define FX_H

#include "ucmp.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

namespace efwk
{

enum class fx_state { disabled, enabled };

struct fx_emit_spark
{
        fx_state state;
        cooldown_stat cdown;
        fx_emit_spark(fx_state new_state, double interval) :
                state(new_state),
                cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasFxEmitSpark, fx_emit_spark, eff);

struct fx_emit_smoke
{
        fx_state state;
        cooldown_stat cdown;
        fx_emit_smoke(fx_state new_state, double interval) :
                state(new_state),
                cdown(interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasFxEmitSmoke, fx_emit_smoke, eff);

struct fx_emit_compound
{
        fx_emit_spark spark;
        fx_emit_smoke smoke;

        fx_emit_compound(fx_state spark_state, double spark_interval,
                         fx_state smoke_state, double smoke_interval) :
                spark(spark_state, spark_interval),
                smoke(smoke_state, smoke_interval)
        {}
};

SFINAE__DECLARE_HAS_MEMBER(HasFxEmitCompound, fx_emit_compound, eff);

template <class T>
// TODO: Instead of TmpAny implement and use TmpOneOnly
using HasFxEmit = TmpAny<HasFxEmitSmoke<T>,
                         HasFxEmitSpark<T>,
                         HasFxEmitCompound<T>>;

}

#endif
