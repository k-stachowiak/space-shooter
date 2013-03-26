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

#include "sys_score.h"

namespace sys {

void score_system::update() {

	// Kind of a short-circuit internal message queue.
	std::multimap<uint64_t, double> score_increments;

	// Detect deaths and store the point increments for them.
	for(auto const& pr : _nodes) {

		auto const& n = pr.second;

		if(n.wellness->is_alive())
			continue;

		uint64_t receiver = n.wellness->get_last_dmg_id();
		double score = _class_score_map.at(n.sc);

		score_increments.insert(make_pair(receiver, score));
	}

	// Grant the score increments to the according nodes.
	for(auto const& pr : score_increments) {

		uint64_t const& id = pr.first;
		double const& increment = pr.second;

		auto nit = _nodes.find(id);
		if(nit == end(_nodes))
			continue;

		auto const& n = nit->second;

		if(n.score)
			*(n.score) += increment;
	}
}

}
