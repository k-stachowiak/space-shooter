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

#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

namespace efwk
{

inline void draw_hud(ALLEGRO_FONT const * const font,
                     const int score,
                     const double health)
{
        static double screen_h = static_cast<double>(cfg::integer("gfx_screen_h"));

        al_draw_textf(font, al_map_rgb_f(1, 1, 1), 10, 10, 0, "Score : %d", score);
        al_draw_textf(font, al_map_rgb_f(1, 0.1, 0.01), 20, screen_h - 30, 0, "Health : %.2f", health);
}

}

#endif
