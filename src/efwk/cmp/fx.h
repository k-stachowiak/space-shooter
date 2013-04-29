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

#ifndef CMP_FX_H
#define CMP_FX_H

#include <memory>
using std::shared_ptr;

#include "../comm.h"

namespace cmp {

class timer;
class shape;

class fx {
public:
        virtual ~fx() {}
        virtual void update(
                        double dt,
                        double health_ratio,
                        double x, double y,
                        shape const& shp,
                        comm::msg_queue& msgs) = 0;
};

shared_ptr<fx> create_smoke_when_hurt(shared_ptr<timer> t, double pain_threshold);
shared_ptr<fx> create_period_smoke(double dt_min, double dt_max);

}

#endif
