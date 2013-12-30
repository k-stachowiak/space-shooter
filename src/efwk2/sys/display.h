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

#include "../cmp/appearance.h"
#include "../cmp/orientation.h"

namespace efwk
{

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
        const auto& appr = ent.appr;
        const auto& ori = ent.ori;

        display_impl(appr, ori, weight);
}

template <class Entity>
typename std::enable_if<!IsDisplayable<Entity>::value, void>::type
display(const Entity&, double) {}

}

#endif
