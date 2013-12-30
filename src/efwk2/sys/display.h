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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "../cmp/appearance.h"
#include "../cmp/orientation.h"
#include "../cmp/shape.h"
#include "../cmp/coll_traits.h"
#include "../cmp/wellness.h"

namespace efwk
{

// Common display routines.
// ========================

inline
void display_shape(double x, double y, double phi,
                   const efwk::shape_segment& seg)
{
        auto transformed = trans(seg, x, y, phi);
        al_draw_line(transformed.a.x, transformed.a.y,
                     transformed.b.x, transformed.b.y,
                     al_map_rgb_f(1, 1, 0), 1);
}

inline
void display_shape(double x, double y, double phi,
                   const efwk::shape_polygon& poly)
{
        std::for_each(begin(poly.segs), begin(poly.segs) + poly.num_segs,
                [x, y, phi] (const efwk::shape_segment& seg) {
                          display_shape(x, y, phi, seg);
                });
}

inline
void display_shape(double x, double y, double phi,
                   const efwk::shape_square& sqr)
{
        auto segs = segments(sqr);
        for (const auto& seg : segs) {
                display_shape(x, y, phi, seg);
        }
}

inline
void display_shape(double x, double y, double,
                   const efwk::shape_circle& cir)
{
        al_draw_circle(x, y, cir.radius, al_map_rgb_f(1, 1, 0), 1);
}

template <class Wellness>
void display_wellness(double x, double y,
                      const Wellness& wlns,
                      ALLEGRO_FONT* font)
{
        al_draw_textf(font, al_map_rgb_f(1, 1, 1), x, y, 0,
                      "H(%.2f)", wlns.get_health());
}

// Regular display routines.
// =========================

void display_impl(const appearance_pixel& appr, const orientation& ori, double weight)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);
        al_draw_pixel(x, y, al_map_rgb_f(appr.r, appr.g, appr.b));
}

void display_impl(const appearance_static_bmp& appr, const orientation& ori, double weight)
{
        ALLEGRO_BITMAP* bmp = appr.current_bitmap;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);

        double phi = ori.interpolate_rot(weight);

        int w = al_get_bitmap_width(bmp);
        int h = al_get_bitmap_height(bmp);
        al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
}

template <class T>
using IsDisplayable = TmpAll<HasAppearance<T>, HasOrientation<T>>;

template <class Entity>
typename std::enable_if<IsDisplayable<Entity>::value, void>::type
display(const Entity& ent, double weight)
{
        display_impl(ent.appr, ent.ori, weight);
}

template <class Entity>
typename std::enable_if<!IsDisplayable<Entity>::value, void>::type
display(const Entity&, double) {}

// Debug display routines.
// =======================

template <class Shape, class Wellness>
void display_dbg_impl(const Shape& shp,
                      const orientation& ori,
                      const Wellness& wlns,
                      double weight,
                      ALLEGRO_FONT* font)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);

        double phi = ori.interpolate_rot(weight);

        display_shape(x, y, phi, shp);
        display_wellness(x, y, wlns, font);
}

template <class T>
using IsDbgDisplayable = TmpAll<HasShape<T>,
                                HasOrientation<T>,
                                HasCollisionTraits<T>,
                                HasWellness<T>>;

template <class Entity>
typename std::enable_if<IsDbgDisplayable<Entity>::value, void>::type
display_dbg(const Entity& ent, double weight, ALLEGRO_FONT* font)
{
        display_dbg_impl(ent.shp, ent.ori, ent.wlns, weight, font);
}

template <class Entity>
typename std::enable_if<!IsDbgDisplayable<Entity>::value, void>::type
display_dbg(const Entity&, double, ALLEGRO_FONT*) {}

}

#endif
