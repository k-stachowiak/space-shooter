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

#ifndef APPR_H
#define APPR_H

#include <allegro5/allegro.h>

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

namespace cmp {

// Helper types.
// -------------

struct frame_def {
        uint32_t index;
        double time;
        frame_def(uint32_t index, double time) : index(index), time(time) {}
        frame_def() : index(-1), time(-1) {}
        frame_def& operator=(const frame_def& rhs) {
                index = rhs.index;
                time = rhs.time;
                return *this;
        }
};

// Interface.
// ----------

class appearance {
public:
        virtual ~appearance() {}
        virtual void update(double dt) = 0;
        virtual void draw(double x, double y, double phi) const = 0;
        virtual void draw_flash(double x, double y, double phi) const = 0;
};

// Constructors.
// -------------

shared_ptr<appearance> create_pixel(double r, double g, double b);

shared_ptr<appearance> create_static_bmp(
                ALLEGRO_BITMAP* bmp,
                ALLEGRO_BITMAP* flash);

shared_ptr<appearance> create_simple_anim(
                ALLEGRO_BITMAP* bmp,
                ALLEGRO_BITMAP* flash,
                uint32_t frame_width,
                uint32_t num_frames,
                vector<frame_def> const& frame_defs,
                int32_t rep_count);

shared_ptr<appearance> create_bin_proxy_appr(
                bool& state,
                shared_ptr<appearance> true_appr,
                shared_ptr<appearance> false_appr);

}

#endif
