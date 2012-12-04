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

#ifndef ALLEGRO_H
#define ALLEGRO_H

#include <stdint.h>

#include <string>
using namespace std;

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "misc/exceptions.h"
#include "states/state.h"

class allegro {

	ALLEGRO_DISPLAY* _display;
	ALLEGRO_EVENT_QUEUE* _event_queue;
	ALLEGRO_TIMER* _timer;

public:
	allegro(uint32_t scr_w, uint32_t scr_h, string title, double fps);
	~allegro();
	ALLEGRO_DISPLAY* get_display();
	void dump_events(state& s, uint32_t& overdue_frames);
	void swap_buffers();

private:
	void handle_event(ALLEGRO_EVENT& ev, state& s, uint32_t& overdue_frame);
};

#endif
