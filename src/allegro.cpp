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

#include "allegro.h"

void allegro::handle_event(ALLEGRO_EVENT& ev, state& s) const {
        switch(ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
                s.sigkill();
                return;

        case ALLEGRO_EVENT_KEY_DOWN:
                s.key_down(ev.keyboard.keycode);
                break;

        case ALLEGRO_EVENT_KEY_UP:
                s.key_up(ev.keyboard.keycode);
                break;

        case ALLEGRO_EVENT_KEY_CHAR:
                s.key_typed(ev.keyboard.unichar);
                break;

        default:
                break;
        }
}

allegro::allegro(uint32_t scr_w, uint32_t scr_h, std::string title) {

        // Initialize allegro.
        // -------------------
        if(!al_init()) {
                throw initialization_error("Failed initializing allegro");
        }

        // Basic IO initialization.
        // ------------------------
        if(!al_install_keyboard()) {
                throw initialization_error("Failed initializing allegro keyboard");
        }

        if(!al_install_mouse()) {
                throw initialization_error("Failed initializing allegro mouse");
        }
        
        if(!al_install_audio()) {
                throw initialization_error("Failed initializing audio");
        }

        // Initialize add-ons.
        // -------------------
        if(!al_init_image_addon()) {
                throw initialization_error("Failed initializing image add-on");
        }

        if(!al_init_primitives_addon()) {
                throw initialization_error("Failed initializing primitives add-on");
        }

        al_init_font_addon();
        if(!al_init_ttf_addon()) {
                throw initialization_error("Failed initializing TTF add-on");
        }

        if(!al_init_acodec_addon()) {
                throw initialization_error("Failed initializing acodec add-on");
        }

        // Display initialization.
        // -----------------------
        al_set_new_display_flags(ALLEGRO_WINDOWED /* | ALLEGRO_OPENGL_3_0 */);
        _display = al_create_display(scr_w, scr_h);
        if(!_display) {
                throw initialization_error("Failed initializing allegro display");
        }
        al_set_window_title(_display, title.c_str());

        // Create and setup the event queue.
        // ---------------------------------
        _event_queue = al_create_event_queue();
        if(!_event_queue) {
                throw initialization_error("Failed initializing allegro event queue");
        }

        // Generic event sources.
        al_register_event_source(_event_queue, al_get_keyboard_event_source());
        al_register_event_source(_event_queue, al_get_mouse_event_source());
        al_register_event_source(_event_queue, al_get_display_event_source(_display));

}

allegro::~allegro() {
        al_destroy_event_queue(_event_queue);
        al_destroy_display(_display);
        al_uninstall_audio();
}

ALLEGRO_DISPLAY* allegro::get_display() {
        return _display;
}

double allegro::current_time() {
        return al_get_time();
}

void allegro::dump_events(state& s) {

        /*
        * Dump all the pending events and send appropriate signals to the state object.
        * Additionally increase the number of the overdue frames on each timer's tick.
        */

        ALLEGRO_EVENT ev;
        while(!al_is_event_queue_empty(_event_queue)) {
                al_get_next_event(_event_queue, &ev);
                handle_event(ev, s);
        }
}

void allegro::swap_buffers() const {
        al_flip_display();
}

double allegro::get_display_refresh_rate() const {
        int result = al_get_display_refresh_rate(_display);
        return double(result);
}

