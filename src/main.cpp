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

#include <iostream>
#include <memory>
using namespace std;

#include "allegro.h"
#include "misc/config.h"
#include "resources/resman.h"
#include "states/state.h"

class application {

	const double _fps;
	const double _spf;
	uint32_t _overdue_frames;

	allegro _allegro;
	resman _resman;

public:
	application()
		: _fps(cfg::real("gfx_fps"))
		, _spf(1.0 / _fps)
		, _overdue_frames(0)
		, _allegro(cfg::integer("gfx_screen_w"), cfg::integer("gfx_screen_h"), "Framework", _fps)
		, _resman(_allegro.get_display()) {
	}

	void loop() {
		unique_ptr<state> current_state = create_test_state(_resman);
		while(current_state) {

			// Handle events.
			_allegro.dump_events(*current_state.get(), _overdue_frames);			

			// See if the state needs change.
			if(current_state->done()) {
				current_state = current_state->next_state();
				_overdue_frames = 0;
				continue;
			}

			// Simulate overdue frames.
			while(_overdue_frames > 0) {
				current_state->frame_logic(_spf);
				--_overdue_frames;
			}

			_allegro.swap_buffers();
		}
	}
};

int main() {
	try {
		cfg::load_from_file("config");
        application app;
		app.loop();
		return 0;
	} catch (initialization_error& e) {
		cerr << "Initialization failed : " << e.what() << endl;
		return 1;
	}
}
