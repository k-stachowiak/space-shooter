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

#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include <vector>
using std::vector;

#include "../efwk/entity_factory.h"
#include "wave.h"

class enemy_manager {
	vector<wave> _waves;
	size_t _current_wave;

public:
	enemy_manager(vector<wave> waves)
	: _waves(waves)
	, _current_wave(0)
	{}

	bool tick(double dt, entity_factory& ef, double screen_w, double screen_h) {

		if(_current_wave >= _waves.size()) {
			return false;
		}

		wave& current = _waves[_current_wave];
		if(!current.tick(dt, ef, screen_w, screen_h)) {
			current.reset();
			_current_wave += 1;
			if(_current_wave == _waves.size()) {
				return false;
			}
		}
		return true;
	}

	void reset() {
		_current_wave = 0;
	}
};

#endif
