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

#ifndef DISPLAY_DBG_H
#define DISPLAY_DBG_H

#include <algorithm>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "../cmp/shape.h"
#include "../cmp/orientation.h"

namespace
{
        void display_shape(double x, double y, double phi,
                           const efwk::shape_segment& seg)
        {
                auto transformed = trans(seg, x, y, phi);
                al_draw_line(transformed.a.x, transformed.a.y,
                             transformed.b.x, transformed.b.y,
                             al_map_rgb_f(1, 1, 0), 1);
        }

        void display_shape(double x, double y, double phi,
                           const efwk::shape_polygon& poly)
        {
                std::for_each(begin(poly.segs), begin(poly.segs) + poly.num_segs,
                        [x, y, phi] (const efwk::shape_segment& seg) {
                                  display_shape(x, y, phi, seg);
                        });
        }

        void display_shape(double x, double y, double phi,
                           const efwk::shape_square& sqr)
        {
                auto segs = segments(sqr);
                for (const auto& seg : segs) {
                        display_shape(x, y, phi, seg);
                }
        }

        void display_shape(double x, double y, double,
                           const efwk::shape_circle& cir)
        {
                al_draw_circle(x, y, cir.radius, al_map_rgb_f(1, 1, 0), 1);
        }
}

namespace efwk
{

template <class Entity>
void display_dbg_impl(const Entity& ent, double weight)
{
        const auto& shp = ent.shp;
        const auto& ori = ent.ori;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);

        double phi = ori.interpolate_rot(weight);

        display_shape(x, y, phi, shp);
}

template <class T>
using IsDbgDisplayable = TmpAll<HasShape<T>, HasOrientation<T>>;

template <class Entity>
typename std::enable_if<IsDbgDisplayable<Entity>::value, void>::type
display_dbg(const Entity& ent, double weight)
{
        display_dbg_impl(ent, weight);
}

template <class Entity>
typename std::enable_if<!IsDbgDisplayable<Entity>::value, void>::type
display_dbg(const Entity&, double)
{
}

}

#endif
