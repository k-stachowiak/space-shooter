/* Copyright (C) 2012 Krzysztof Stachowiak */

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

#ifndef RESMAN_H
#define RESMAN_H

#include <stdint.h>

#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
using namespace std;

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../misc/exceptions.h"

// TODO: Create a "complex resource" : sequence/animation
//       - it will store the bitmap, frame width/count / etc.
//         and carry it around, e.g. to the factory routine.
//	   ...have "animation -> appearance" and
//	           "bitmap -> appearance" functions...

enum class res_id : int {

	// Prepared resources.
	PLAYER_SHIP,
	PLAYER_BULLET,
	ENEMY_EYE,
	ENEMY_BOMBER,
	EYE_BULLET,
	MISSILE,
	TINY_FONT,
	FONT,
	DEBRIS1,
	DEBRIS2,
	DEBRIS3,
	DEBRIS4,
	DEBRIS5,
	HEALTH,

	// Generated bitmaps.
	SMOKE,
	SMOKE_SMALL,
	SMOKE_BIG,
	EXPLOSION,
};

class resman {

	struct bitmap_deleter {
		void operator()(ALLEGRO_BITMAP* bitmap) {			
			al_destroy_bitmap(bitmap);
		}
	};

	struct font_deleter {
		void operator()(ALLEGRO_FONT* font) {
			al_destroy_font(font);
		}
	};

	// Helper typedef.
	typedef unique_ptr<ALLEGRO_BITMAP, bitmap_deleter> p_bmp;

	ALLEGRO_DISPLAY* _dpy;

	map<res_id, p_bmp> _bitmaps;
	map<res_id, unique_ptr<ALLEGRO_FONT, font_deleter>> _fonts;

public:
	resman(ALLEGRO_DISPLAY* dpy);
	ALLEGRO_BITMAP* get_bitmap(res_id id) const;
	ALLEGRO_FONT* get_font(res_id id) const;

private:
	void add_bitmap(res_id id, string path);
	void add_font(res_id id, string path, int size);

	void expand_fade(
			res_id id,
			string path,
			uint32_t num_frames,
			double exp_factor);

	void fade_frames(res_id id,
			 string path,
			 uint32_t num_frames);

	void scaled_copy(res_id id, res_id original, double scale);
};

#endif
