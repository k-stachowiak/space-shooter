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

#ifndef SYS_BASE_H
#define SYS_BASE_H

#include <stdint.h>

namespace sys {

template<typename SYS>
void remove_node(SYS& sys, uint64_t id) {
	for(auto n = begin(sys._nodes); n != end(sys._nodes); ++n) {
		if(n->id == id) {
			*n = sys._nodes.back();
			sys._nodes.pop_back();
			--n;
		}
	}
}

class system {
protected:
	bool _debug_mode;
public:
	void set_debug_mode(bool debug_mode) { _debug_mode = debug_mode; }
};

}

#endif
