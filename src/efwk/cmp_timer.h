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

#ifndef CMP_TIMER_H
#define CMP_TIMER_H

#include <memory>
using std::shared_ptr;

namespace cmp {

class timer {
protected:
	uint32_t _ticks;
	double _counter;
public:
	virtual ~timer() {}
	uint32_t get_ticks() const { return _ticks; }
	void clear() { _ticks = 0; }
	void reset() { _ticks = _counter = 0; }
	virtual void update(double dt) = 0;
};

shared_ptr<timer> create_const_int_timer(double interval);

}

#endif
