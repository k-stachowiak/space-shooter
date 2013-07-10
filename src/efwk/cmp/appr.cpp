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

#include <array>

#include "../../misc/config.h"
#include "appr.h"

using namespace std;

namespace cmp {

class pixel : public appearance {
        double _r, _g, _b;
public:
        pixel(double r, double g, double b) : _r(r), _g(g), _b(b) {}

        void update(double dt) {}

        void draw(double x, double y, double phi) const {
                al_draw_pixel(x + 0.5, y + 0.5, al_map_rgb_f(_r, _g, _b));
        }

        void draw_flash(double x, double y, double phi) const {}
};

std::shared_ptr<appearance> create_pixel(double r, double g, double b) {
        return std::shared_ptr<appearance>(new pixel(r, g, b));
}

class static_bmp : public appearance {
        ALLEGRO_BITMAP* _bmp;
        ALLEGRO_BITMAP* _flsh;
public:
        static_bmp(ALLEGRO_BITMAP* bmp, ALLEGRO_BITMAP* flsh) : _bmp(bmp), _flsh(flsh) {}
        void update(double) {}

        void draw(double x, double y, double phi) const {
                int w = al_get_bitmap_width(_bmp);
                int h = al_get_bitmap_height(_bmp);
                al_draw_rotated_bitmap(_bmp,
                                w / 2, h / 2,
                                x, y, phi, 0);
        }

        void draw_flash(double x, double y, double phi) const {
                int w = al_get_bitmap_width(_flsh);
                int h = al_get_bitmap_height(_flsh);
                al_draw_rotated_bitmap(_flsh,
                                w / 2, h / 2,
                                x, y, phi, 0);
        }
};

std::shared_ptr<appearance> create_static_bmp(
                ALLEGRO_BITMAP* bmp,
                ALLEGRO_BITMAP* flash) {
        return std::shared_ptr<appearance>(new static_bmp(bmp, flash));
}

class simple_anim : public appearance {

        // The original bitmap - the frames' atlas.
        ALLEGRO_BITMAP* _bitmap;

        // The flashed variant of the bitmap.
        ALLEGRO_BITMAP* _flash;

        // The information about the particular frames.
        uint32_t _frame_width;
        uint32_t _num_frames;
        std::array<ALLEGRO_BITMAP*, cfg::type_bounds::max_num_frames> _frame_images;
        std::array<ALLEGRO_BITMAP*, cfg::type_bounds::max_num_frames> _flash_images;

        // The information about the frame definitions.
        uint32_t _num_defs;
        std::array<frame_def, cfg::type_bounds::max_frame_defs> _frame_defs;

        // How many times should we repeat?
        int _rep_count;

        // The animation's state.
        uint32_t _current_def;
        double _time_to_switch;
        double _done;

public:
        simple_anim(
                ALLEGRO_BITMAP* bitmap,
                ALLEGRO_BITMAP* flash,
                uint32_t frame_width,
                uint32_t num_frames,
                std::vector<frame_def> const& frame_defs,
                int rep_count)
        : _bitmap(bitmap)
        , _flash(flash)
        , _frame_width(frame_width)
        , _num_frames(num_frames)
        , _rep_count(rep_count)
        , _done(false) {

                // Generate the frame's sub-bitmaps
                for(uint32_t frame = 0; frame < _num_frames; ++frame) {
                        _frame_images[frame] = al_create_sub_bitmap(
                                _bitmap,
                                frame * _frame_width, 0,
                                _frame_width, al_get_bitmap_height(_bitmap));

                        _flash_images[frame] = al_create_sub_bitmap(
                                _flash,
                                frame * _frame_width, 0,
                                _frame_width, al_get_bitmap_height(_flash));
                }

                // Store the definitions.
                _num_defs = frame_defs.size();
                for(uint32_t i = 0; i < _num_defs; ++i)
                        _frame_defs[i] = frame_defs[i];

                // Setup the animation state.
                _current_def = 0;
                _time_to_switch = _frame_defs[_current_def].time;
        }

        ~simple_anim() {
                for(uint32_t i = 0; i < _num_frames; ++i) {
                        al_destroy_bitmap(_frame_images[i]);
                        al_destroy_bitmap(_flash_images[i]);
                }
        }

        void update(double dt) {

                // Progress time.
                _time_to_switch -= dt;

                // Switch frame if necessary.
                if(!_done && (_time_to_switch <= 0.0)) {

                        // Are we done?
                        if(_current_def == _num_defs - 1) {
                                if(_rep_count > 0)
                                        --_rep_count;

                                if(_rep_count == 0) {
                                        _done = true;
                                        return;
                                }
                        }

                        // How much have wee exceeded the frame time?
                        double rest = -_time_to_switch;

                        _current_def = (_current_def + 1) % _num_defs;
                        _time_to_switch = _frame_defs[_current_def].time;

                        // Take the extended time into account.
                        _time_to_switch -= rest;
                }
        }

        void draw(double x, double y, double phi) const {
                uint32_t current_index = _frame_defs[_current_def].index;
                ALLEGRO_BITMAP* bmp = _frame_images[current_index];
                int w = al_get_bitmap_width(bmp);
                int h = al_get_bitmap_height(bmp);
                al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
        }

        void draw_flash(double x, double y, double phi) const {
                uint32_t current_index = _frame_defs[_current_def].index;
                ALLEGRO_BITMAP* bmp = _flash_images[current_index];
                int w = al_get_bitmap_width(bmp);
                int h = al_get_bitmap_height(bmp);
                al_draw_rotated_bitmap(bmp, w / 2, h / 2, x, y, phi, 0);
        }

};

std::shared_ptr<appearance> create_simple_anim(
                ALLEGRO_BITMAP* bmp,
                ALLEGRO_BITMAP* flash,
                uint32_t frame_width,
                uint32_t num_frames,
                vector<frame_def> const& frame_defs,
                int32_t rep_count) {

        return std::shared_ptr<appearance>(new simple_anim(
                        bmp,
                        flash,
                        frame_width,
                        num_frames,
                        frame_defs,
                        rep_count));
}

class bin_proxy : public appearance {

        bool& _state;
        std::shared_ptr<appearance> _true_appr;
        std::shared_ptr<appearance> _false_appr;

        std::shared_ptr<appearance> current_appr() const {
                return _state ? _true_appr : _false_appr;
        }

public:
        bin_proxy(bool& state,
                  shared_ptr<appearance> true_appr,
                  shared_ptr<appearance> false_appr)
        : _state(state)
        , _true_appr(true_appr)
        , _false_appr(false_appr)
        {}

        void update(double dt) {
                current_appr()->update(dt);
        }

        void draw(double x, double y, double phi) const {
                current_appr()->draw(x, y, phi);
        }

        void draw_flash(double x, double y, double phi) const {
                current_appr()->draw_flash(x, y, phi);
        }
};

std::shared_ptr<appearance> create_bin_proxy_appr(
                bool& state,
                std::shared_ptr<appearance> true_appr,
                std::shared_ptr<appearance> false_appr) {

        return std::shared_ptr<appearance>(
                        new bin_proxy(state, true_appr, false_appr));
}

}
