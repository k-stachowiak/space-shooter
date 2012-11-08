#ifndef ALLEGRO_H
#define ALLEGRO_H

#include <stdint.h>

#include <string>
using namespace std;

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "exceptions.h"
#include "state.h"

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
