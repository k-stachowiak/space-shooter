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

#include <iostream>
using namespace std;

#include "sys_sound.h"

map<ALLEGRO_SAMPLE*, ALLEGRO_SAMPLE_ID> spl_id_map;

namespace sys {

void sound_system::update(double dt) {

        // Handle local queue.
        _noise_queue.visit(dt, [this](res_id rid) {
                ALLEGRO_SAMPLE* sample = _resman.get_sample(rid);
                al_stop_sample(&spl_id_map[sample]);
                al_play_sample(
                        sample,
                        1, 0, 1,
                        ALLEGRO_PLAYMODE_ONCE,
                        &spl_id_map[sample]);
        });

        // Acquire new events.
        for(auto const& n : _nodes) {
                _noise_queue.consume(*n.nqueue);
        }
}

}

