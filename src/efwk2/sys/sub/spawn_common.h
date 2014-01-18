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

#ifndef SPAWN_COMMON_H
#define SPAWN_COMMON_H

namespace efwk
{

template <class XDirDist, class YDirDist,
          class XVelDist, class YVelDist>
std::pair<double, double> gen_rnd_vel(
                XDirDist& x_dir_dist, YDirDist& y_dir_dist,
                XVelDist& x_vel_dist, YVelDist& y_vel_dist)
{
        const double x_dir = x_dir_dist(rnd::engine);
        const double y_dir = y_dir_dist(rnd::engine);
        return std::make_pair(
                x_vel_dist(rnd::engine) * (x_dir * 2.0 - 1.0),
                y_vel_dist(rnd::engine) * (y_dir * 2.0 - 1.0));
}

}

#endif
