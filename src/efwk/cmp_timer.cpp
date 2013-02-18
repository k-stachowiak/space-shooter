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

#include "cmp_timer.h"

namespace cmp {

class const_int_timer : public timer {
	double _interval;
public:
	const_int_timer(double interval)
	: _interval(interval)
	{
		_counter = interval;
		_ticks = 0;
	}

	void update(double dt) {
		_counter -= dt;
		if(_counter < 0.0) {
			++_ticks;
			_counter += _interval;
		}
	}
};

shared_ptr<timer> create_const_int_timer(double interval) {
	return shared_ptr<timer>(new const_int_timer(interval));
}

}
