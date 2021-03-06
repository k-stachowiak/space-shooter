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

#ifndef ALLEGRO_H
#define ALLEGRO_H

#include <stdint.h>

#include <string>
#include <memory>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "misc/exceptions.h"
#include "states/state.h"

class allegro {

        struct display_deleter
        {
                void operator()(ALLEGRO_DISPLAY* d)
                {
                        al_destroy_display(d);
                }
        };

        struct queue_deleter
        {
                void operator()(ALLEGRO_EVENT_QUEUE* eq)
                {
                        al_destroy_event_queue(eq);
                }
        };

        std::unique_ptr<ALLEGRO_DISPLAY, display_deleter> _display;
        std::unique_ptr<ALLEGRO_EVENT_QUEUE, queue_deleter> _event_queue;

        void handle_event(ALLEGRO_EVENT& ev, state& s) const;

public:
        allegro(uint32_t scr_w, uint32_t scr_h, std::string title);
        ALLEGRO_DISPLAY* get_display();
        double current_time();
        void dump_events(state& s);
        void swap_buffers() const;
        double get_display_refresh_rate() const;
};

#endif
