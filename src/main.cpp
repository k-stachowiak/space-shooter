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
#include "script/scriptman.h"
#include "script/expect.h"
#include "resources/resman.h"
#include "states/state.h"
#include "misc/logger.h"

// TODO:
// - Highscore
// - Use consisten namespacing strategy
// - Why self damage?
//   - np. wróg na ekranie i odpalam rakiety.

using namespace res;

class application {

        script::scriptman _sman;
        cfg::loader _cfg_RAII_plug;

        const double _fps;
        const double _spf;

        allegro _allegro;
        resman _resman;

public:
        application()
        : _sman({ "config", "waves" })
        , _cfg_RAII_plug(_sman.get_dom("config"))
        , _fps(cfg::real("gfx_fps"))
        , _spf(1.0 / _fps)
        , _allegro(
                cfg::integer("gfx_screen_w"),
                cfg::integer("gfx_screen_h"),
                "Framework",
                _fps)
        , _resman(
                _allegro.get_display(),
                {
                        { res_id::BULLET_SHOOT,"data/bullet_shoot.wav" },
                        { res_id::MISSILE_SHOOT, "data/missile_shoot_2.wav" },
                        { res_id::WEAPON_PICKUP, "data/click.wav" },
                        { res_id::WELLNESS_PICKUP, "data/wellness.wav" },
                        { res_id::EXPLOSION_SND, "data/explosion.wav" },
                        { res_id::INGAME_MUSIC, "data/OveMelaaTranceBitBit.ogg" }
                })
        {}

        void loop() {
                uint32_t overdue_frames;
                unique_ptr<state> current_state = create_menu_state(_resman, _sman);
                while(current_state) {

                        // Handle events.
                        _allegro.dump_events(*current_state.get(), overdue_frames);

                        // See if the state needs change.
                        if(current_state->done()) {
                                current_state = current_state->next_state();
                                overdue_frames = 0;
                                continue;
                        }

                        // Simulate overdue frames.
                        while(overdue_frames > 0) {
                                current_state->frame_logic(_spf);
                                --overdue_frames;
                        }

                        _allegro.swap_buffers();
                }
        }
};

int main() {

        script::test_expect();

        try {
                logger::logger::instance().trace("BEGIN program.");

                application app;
                app.loop();

                logger::logger::instance().trace("END program.");
                logger::logger::instance().flush();

                return 0;
        } catch (initialization_error& e) {
                cerr << "Initialization failed : " << e.what() << endl;
                return 1;
        }

}
