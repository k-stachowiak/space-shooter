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

struct fx_emit_spark
{
        cooldown_stat cdown;
        fx_emit_spark(double interval) : cdown(interval) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasFxEmitSpark, fx_emit_spark, eff);

struct fx_emit_smoke
{
        cooldown_stat cdown;
        fx_emit_smoke(double interval) : cdown(interval) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasFxEmitSmoke, fx_emit_smoke, eff);

template <class T>
// TODO: Instead of TmpAny implement and use TmpOneOnly
using HasFxEmit = TmpAny<HasFxEmitSmoke<T>,
                         HasFxEmitSpark<T>>;

}

#endif
