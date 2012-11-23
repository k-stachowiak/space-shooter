#include <iostream>
#include <memory>
using namespace std;

#include "config.h"
#include "allegro.h"
#include "resources/resman.h"
#include "state.h"

/*
 * TODO:
 * - Player shield bar and shield pickups.
 * - Use common random generator engine.
 */

class application {
	config _config;

	double _fps;
	double _spf;
	uint32_t _overdue_frames;

	allegro _allegro;
	resman _resman;

public:
	application()
		: _fps(_config.get_fps())
		, _spf(1.0 / _fps)
		, _overdue_frames(0)
		, _allegro(_config.get_screen_w(), _config.get_screen_h(), "Framework", _fps)
		, _resman(_allegro.get_display()) {
	}

	void loop() {
		// unique_ptr<state> current_state = create_game_state(_config, _resman);
		unique_ptr<state> current_state = create_test_state(_config, _resman);
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
		application app;
		app.loop();
		return 0;
	} catch (initialization_error e) {
		cerr << "Initialization failed : " << e.what() << endl;
		return cin.get();
	}
}
