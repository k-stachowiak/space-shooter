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

#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <allegro5/allegro.h>

#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

namespace efwk
{

struct appearance_static_bmp
{
        ALLEGRO_BITMAP* current_bitmap;
        appearance_static_bmp(ALLEGRO_BITMAP* bitmap) : current_bitmap(bitmap) {}
        void update(double) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasAppearanceStaticBmp, appearance_static_bmp, appr);

class appearance_animated_bmp
{
        // The original bitmap containing all the frames.
        ALLEGRO_BITMAP* m_bitmap;

        // Frame information.
        int m_frame_width;
        int m_num_frames;
        std::array<ALLEGRO_BITMAP*, cfg::type_bounds::max_num_frames> m_frame_images;

        // The information about the frame definitions.
        int m_num_defs;
        std::array<std::pair<int, double>, cfg::type_bounds::max_frame_defs> m_frame_defs;

        // The repeat configuration.
        int m_repeat_count;

        // The animation state.
        int m_current_def;
        double m_time_to_switch;
        bool m_done;

public:
        template <class FrameCollection>
        appearance_animated_bmp(
                        ALLEGRO_BITMAP* bitmap,
                        int frame_width,
                        int num_frames,
                        FrameCollection frame_defs,
                        int repeat_count) :
                m_bitmap(bitmap),
                m_frame_width(frame_width),
                m_num_frames(num_frames),
                m_repeat_count(repeat_count),
                m_done(false)
        {
                // Generate the sub bitmaps.
                for (int i = 0; i < m_num_frames; ++i) {
                        m_frame_images[i] = al_create_sub_bitmap(
                                        m_bitmap,
                                        i * m_frame_width,
                                        0,
                                        m_frame_width,
                                        al_get_bitmap_height(m_bitmap));
                }

                // Store the definitions.
                m_num_defs = std::distance(begin(frame_defs), end(frame_defs));
                auto it = begin(frame_defs);
                for (int i = 0; i < m_num_defs; ++i) {
                        m_frame_defs[i] = *(it++);
                }

                // Setup the animation state.
                m_current_def = 0;
                m_time_to_switch = m_frame_defs[m_current_def].second;
        }

        appearance_animated_bmp(const appearance_animated_bmp& copy) :
                        m_bitmap(copy.m_bitmap),
                        m_frame_width(copy.m_frame_width),
                        m_num_frames(copy.m_num_frames),
                        m_num_defs(copy.m_num_defs),
                        m_frame_defs(copy.m_frame_defs),
                        m_repeat_count(copy.m_repeat_count),
                        m_current_def(copy.m_current_def),
                        m_time_to_switch(copy.m_time_to_switch),
                        m_done(copy.m_done)
        {
                for (int i = 0; i < m_num_frames; ++i)
                        m_frame_images[i] = al_clone_bitmap(copy.m_frame_images[i]);
        }

        appearance_animated_bmp(appearance_animated_bmp&& moved) = delete;

        ~appearance_animated_bmp()
        {
                for (int i = 0; i < m_num_frames; ++i)
                        al_destroy_bitmap(m_frame_images[i]);
        }

        appearance_animated_bmp& operator=(appearance_animated_bmp rhs)
        {
                swap(*this, rhs);
                return *this;
        }

        appearance_animated_bmp& operator=(appearance_animated_bmp&& rhs) = delete;

        friend void swap(appearance_animated_bmp& lhs,
                         appearance_animated_bmp& rhs)
        {
                using std::swap;
                swap(lhs.m_bitmap, rhs.m_bitmap);
                swap(lhs.m_frame_width, rhs.m_frame_width);
                swap(lhs.m_num_frames, rhs.m_num_frames);
                swap(lhs.m_frame_images, rhs.m_frame_images);
                swap(lhs.m_num_defs, rhs.m_num_defs);
                swap(lhs.m_frame_defs, rhs.m_frame_defs);
                swap(lhs.m_repeat_count, rhs.m_repeat_count);
                swap(lhs.m_current_def, rhs.m_current_def);
                swap(lhs.m_time_to_switch, rhs.m_time_to_switch);
                swap(lhs.m_done, rhs.m_done);
        }

        void update(double dt)
        {
                // Progress time.
                m_time_to_switch -= dt;

                // Switch the current frame if necessary.
                if (!m_done && (m_time_to_switch <= 0.0)) {

                        // Are we done?
                        if (m_current_def == m_num_defs -1) {
                                if (m_repeat_count > 0)
                                        -- m_repeat_count;

                                if (m_repeat_count == 0) {
                                        m_done = true;
                                        return;
                                }
                        }

                        // How much do we exceed the frame time?
                        double remainder = -m_time_to_switch;

                        m_current_def = (m_current_def + 1) % m_num_defs;
                        m_time_to_switch = m_frame_defs[m_current_def].second;

                        // Take the extended time into account.
                        m_time_to_switch -= remainder;
                }
        }

        ALLEGRO_BITMAP* current_bitmap() const
        {
                int current_index = m_frame_defs[m_current_def].first;
                ALLEGRO_BITMAP* bitmap = m_frame_images[current_index];
                return bitmap;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasAppearanceAnimatedBmp, appearance_animated_bmp, appr);

struct appearance_pixel
{
        double r, g, b;
        appearance_pixel(double new_r, double new_g, double new_b) :
                r(new_r), g(new_g), b(new_b)
        {}

        void update(double) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasAppearancePixel, appearance_pixel, appr);

template <class T>
using HasAppearance = TmpAny<HasAppearanceStaticBmp<T>,
                             HasAppearanceAnimatedBmp<T>,
                             HasAppearancePixel<T>>;

}

#endif /* APPEARANCE_H_ */
