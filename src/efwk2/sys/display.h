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

#include "../../misc/logger.h"

namespace efwk
{

// Common display routines.
// ========================

inline
void display_shape(const efwk::orientation& ori, const efwk::shape_segment& seg)
{
        const auto transformed = trans(seg, ori);
        al_draw_line(transformed.a.x, transformed.a.y,
                     transformed.b.x, transformed.b.y,
                     al_map_rgb_f(1, 1, 0), 1);
}

inline
void display_shape(const efwk::orientation& ori, const efwk::shape_polygon& poly)
{
        for_each_segment(poly, [ori](const efwk::shape_segment& seg)
        {
                display_shape(ori, seg);
        });
}

inline
void display_shape(const efwk::orientation& ori, const efwk::shape_square& sqr)
{
        for_each_segment(sqr, [ori](const efwk::shape_segment& seg)
        {
                display_shape(ori, seg);
        });
}

inline
void display_shape(const efwk::orientation& ori, const efwk::shape_circle& cir)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        al_draw_circle(x, y, cir.radius, al_map_rgb_f(1, 1, 0), 1);
}

struct shape_displayer
{
        const efwk::orientation& cpd_ori;

        template <class Shape>
        void operator()(int,
                        const Shape& current_shp,
                        const efwk::orientation& current_ori)
        {
                orientation composed_ori = compose(current_ori, cpd_ori);
                display_shape(composed_ori, current_shp);
        }
};

template <class... Shapes>
void display_shape(const efwk::orientation& ori, const efwk::shape_compound<Shapes...>& cpd)
{
        shape_displayer displayer { ori };
        for_each_shape(cpd, displayer);
}

template <class Wellness>
void display_wellness(const efwk::orientation& ori, const Wellness& wlns, ALLEGRO_FONT* font)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        al_draw_textf(font, al_map_rgb_f(1, 1, 1), x, y, 0,
                      "H(%.2f)", wlns.get_health());
}

// Regular display routines.
// =========================

inline
void display_impl(const appearance_pixel& appr, const orientation& ori, double weight)
{
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);
        al_draw_pixel(x, y, al_map_rgb_f(appr.r, appr.g, appr.b));
}

inline
void display_impl(const appearance_static_bmp& appr, const orientation& ori, double weight)
{
        ALLEGRO_BITMAP* bmp = appr.current_bitmap;

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);

        const double phi = ori.interpolate_rot(weight);

        const int w = al_get_bitmap_width(bmp);
        const int h = al_get_bitmap_height(bmp);
        al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
}

inline
void display_impl(const appearance_animated_bmp& appr,
                  const orientation& ori,
                  double weight)
{
        ALLEGRO_BITMAP* bmp = appr.current_bitmap();

        double x, y;
        std::tie(x, y) = ori.interpolate_loc(weight);

        const double phi = ori.interpolate_rot(weight);

        const int w = al_get_bitmap_width(bmp);
        const int h = al_get_bitmap_height(bmp);
        al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
}

template <class Appr1, class Appr2>
void display_impl(const appearance_bin_proxy<Appr1, Appr2>& appr,
                  const orientation& ori,
                  double weight)
{
        if (appr.state == bp_state::first)
                display_impl(appr.appr1, ori, weight);
        else
                display_impl(appr.appr2, ori, weight);
}

template <class T>
using IsDisplayable = TmpAll<HasAppearance<T>, HasOrientation<T>>;

template <class Entity>
typename std::enable_if<IsDisplayable<Entity>::value, void>::type
display(const Entity& ent, double weight)
{
        display_impl(ent.appr, ent.ori, weight);
}

// TODO: Implement displaying of the bin_proxy shape.

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

        display_shape(ori, shp);
        display_wellness(ori, wlns, font);
}

template <class Shape1, class Shape2, class Wellness>
void display_dbg_impl(const shape_bin_proxy<Shape1, Shape2>& shp,
                      const orientation& ori,
                      const Wellness& wlns,
                      double weight,
                      ALLEGRO_FONT* font)
{
        if (shp.state == bp_state::first)
                display_dbg_impl(shp.shp1, ori, wlns, weight, font);
        else
                display_dbg_impl(shp.shp2, ori, wlns, weight, font);
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

// Appearance update routines.
// ===========================

template <class Appearance>
void display_update_impl(Appearance& appr, double dt)
{
        appr.update(dt);
}

template <class T>
using IsApprUpdatable = HasAppearance<T>;

template <class Entity>
typename std::enable_if<IsApprUpdatable<Entity>::value, void>::type
display_update(Entity& ent, double dt)
{
        display_update_impl(ent.appr, dt);
}

template <class Entity>
typename std::enable_if<!IsApprUpdatable<Entity>::value, void>::type
display_update(Entity&, double) {}

}

#endif
