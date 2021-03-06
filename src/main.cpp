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

// TODO:
// - Introduce units to the components, and improve the apis by the means of the type system.
// - Introduce "script" component with scripted or hard coded "event handling"

#include <iostream>
#include <memory>

#include "allegro.h"
#include "misc/config.h"
#include "script/scriptman.h"
#include "resources/resman.h"
#include "states/state.h"
#include "misc/logger.h"

class application {

        script::scriptman _sman;
        cfg::loader _cfg_RAII_plug;

        const double _fps;
        const double _spf;

        allegro _allegro;
        res::resman _resman;

public:
        application()
        : _sman({ "config", "waves" })
        , _cfg_RAII_plug(_sman)
        , _fps(cfg::real("gfx_fps"))
        , _spf(1.0 / _fps)
        , _allegro(
                cfg::integer("gfx_screen_w"),
                cfg::integer("gfx_screen_h"),
                "Framework")
        , _resman(
                _allegro.get_display(),
                {
                        { res::res_id::BULLET_SHOOT,"data/bullet_shoot.wav" },
                        { res::res_id::MISSILE_SHOOT, "data/missile_shoot.wav" },
                        { res::res_id::WEAPON_PICKUP, "data/click.wav" },
                        { res::res_id::WELLNESS_PICKUP, "data/wellness.wav" },
                        { res::res_id::EXPLOSION_SND, "data/explosion.wav" },
                        { res::res_id::INGAME_MUSIC, "data/1_minute.ogg" }
                })
        {}

        void loop() {

                const double dt = _spf;
                const double max_frame_time = 0.25;

                double t = 0;
                double current_time = _allegro.current_time();
                double accumulator = 0.0;

                std::unique_ptr<state> current_state = create_menu_state(_resman, _sman);

                while(current_state) {

                        // Logic part.
                        double new_time = _allegro.current_time();
                        double frame_time = new_time - current_time;

                        if(frame_time > max_frame_time)
                                frame_time = max_frame_time;

                        current_time = new_time;
                        accumulator += frame_time;

                        while (accumulator >= dt) {
                                _allegro.dump_events(*current_state.get());
                                current_state->update(t, dt);
                                t += dt;
                                accumulator -= dt;
                        }

                        // Display part.
                        const double weight = accumulator / dt;
                        current_state->draw(weight);
                        _allegro.swap_buffers();

                        // State switch part.
                        if(current_state->done()) {
                                current_state = current_state->next_state();
                        }
                }
        }
};

int main() {

        try {
                logger::logger::instance().trace("BEGIN program.");

                application app;
                app.loop();

                logger::logger::instance().trace("END program.");
                logger::logger::instance().flush();

                return 0;

        } catch (initialization_error& e) {
                std::cerr << "Initialization failed : " << e.what() << std::endl;
                return 1;
        }

}
