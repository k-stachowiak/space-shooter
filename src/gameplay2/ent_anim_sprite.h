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

#ifndef ENT_ANIM_SPRITE_H
#define ENT_ANIM_SPRITE_H

namespace gplay
{

struct anim_sprite
{
        long id;
        const char* type_id;
        double ttl;
        efwk::appearance_animated_bmp appr;
        efwk::orientation ori;

        template <class FrameCollection>
        anim_sprite(const long new_id,
                    const double new_ttl,
                    ALLEGRO_BITMAP* const bitmap,
                    const int frame_width,
                    const int num_frames,
                    const FrameCollection frame_defs,
                    const int repeat_count,
                    const double x,
                    const double y) :
                id(new_id),
                type_id("anim_sprite"),
                ttl(new_ttl),
                appr(bitmap, frame_width, num_frames, frame_defs, repeat_count),
                ori(x, y, 0)
        {}
};

}

#endif
