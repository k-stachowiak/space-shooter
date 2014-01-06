/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#ifndef ENT_SPARK_H
#define ENT_SPARK_H

#include "../efwk2/cmp/appearance.h"
#include "../efwk2/cmp/dynamics.h"
#include "../efwk2/cmp/orientation.h"

namespace gplay
{

struct spark
{
        long id;
        const char* type_id;
        double ttl;
        efwk::appearance_pixel appr;
        efwk::const_vel_dynamics dyn;
        efwk::orientation ori;

        spark(const long new_id,
              const double new_ttl,
              const std::array<double, 3> rgb,
              const double vx,
              const double vy,
              const double x,
              const double y) :
                id(new_id),
                type_id("spark"),
                ttl(new_ttl),
                appr(rgb[0], rgb[1], rgb[2]),
                dyn(vx, vy),
                ori(x, y, 0)
        {}
};

}

#endif
