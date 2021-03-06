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

#ifndef RESMAN_H
#define RESMAN_H

#include <stdint.h>

#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h> 
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../misc/exceptions.h"

namespace res {

enum class res_id : int {

        // Prepared resources.
        PLAYER_SHIP,
        PLAYER_SHIP_FLASH,
        ENEMY_LIGHT_FIGHTER,
        ENEMY_LIGHT_FIGHTER_FLASH,
        ENEMY_HEAVY_FIGHTER,
        ENEMY_HEAVY_FIGHTER_FLASH,
        ENEMY_LIGHT_BOMBER,
        ENEMY_LIGHT_BOMBER_FLASH,
        ENEMY_HEAVY_BOMBER,
        ENEMY_HEAVY_BOMBER_FLASH,
        BULLET_1,
        BULLET_2,
        BULLET_3,
        BULLET_4,
        BULLET_5,
        MISSILE,
        B_UPGRADE,
        M_UPGRADE,
        TINY_FONT,
        FONT,
        DEBRIS1,
        DEBRIS2,
        DEBRIS3,
        DEBRIS4,
        DEBRIS5,
        HEALTH,
        BATTERY,
        HUD_BG,
        DIODE_ON,
        DIODE_OFF,

        // Generated bitmaps.
        SMOKE,
        SMOKE_SMALL,
        SMOKE_BIG,
        EXPLOSION,

        // Sounds.
        BULLET_SHOOT,
        MISSILE_SHOOT,
        WEAPON_PICKUP,
        WELLNESS_PICKUP,
        EXPLOSION_SND,
        INGAME_MUSIC
};


class resman {

        struct bitmap_deleter {
                void operator()(ALLEGRO_BITMAP* bmp) const {
                        al_destroy_bitmap(bmp);
                }
        };

        struct font_deleter {
                void operator()(ALLEGRO_FONT* font) const {
                        al_destroy_font(font);
                }
        };

        struct sample_deleter {
                void operator()(ALLEGRO_SAMPLE* sample) const {
                        al_destroy_sample(sample);
                }
        };

        // Helper typedef.
        typedef std::unique_ptr<ALLEGRO_BITMAP, bitmap_deleter> p_bmp;
        typedef std::unique_ptr<ALLEGRO_FONT, font_deleter> p_font;
        typedef std::unique_ptr<ALLEGRO_SAMPLE, sample_deleter> p_sample;

        ALLEGRO_DISPLAY* _dpy;

        std::map<res_id, p_bmp> _bitmaps;
        std::map<res_id, p_font> _fonts;
        std::map<res_id, p_sample> _samples;

public:
        resman(ALLEGRO_DISPLAY* dpy,
                        std::map<res::res_id, std::string> samples);
        ~resman() { _bitmaps.clear(); _fonts.clear(); }
        ALLEGRO_BITMAP* get_bitmap(res::res_id id) const;
        ALLEGRO_FONT* get_font(res::res_id id) const;
        ALLEGRO_SAMPLE* get_sample(res::res_id id) const;

private:
        void add_bitmap(res::res_id id, std::string const& path);
        void add_font(res::res_id id, std::string const& path, int size);
        void add_sample(res::res_id id, std::string const& path);

        void expand_fade(
                        res::res_id id,
                        std::string path,
                        uint32_t num_frames,
                        double exp_factor);

        void fade_frames(res::res_id id,
                        std::string path,
                         uint32_t num_frames);

        void scaled_copy(res::res_id id, res::res_id original, double scale);

        void flash(res::res_id id, res::res_id original);
};

} // namespace res

#endif
