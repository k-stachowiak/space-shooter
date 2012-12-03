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

#include "../exceptions.h"

enum class res_id : int {

	// Prepared resources.
	PLAYER_SHIP,
	PLAYER_BULLET,
	ENEMY_EYE,
	ENEMY_BOMBER,
	EYE_BULLET,
	MISSILE,
	EXPLOSION,
	SMOKE,
	FONT,
	DEBRIS1,
	DEBRIS2,
	DEBRIS3,
	DEBRIS4,
	DEBRIS5,
	HEALTH,

	// Generated bitmaps.
	SMOKE_1,
	EXPLOSION_1,
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
	void add_font(res_id id, string path);

	void gen_expand_fade(
			res_id id,
			string path,
			uint32_t num_frames,
			double exp_factor);

	void fade_frames(res_id id,
			 string path,
			 uint32_t num_frames);
};

#endif
