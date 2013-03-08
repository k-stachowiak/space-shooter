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

#include "../misc/config.h"
#include "sys_pain.h"

namespace sys {

static bool is_dont_care(shared_ptr<cmp::collision_profile> cp) {
	bool ignore_team = cp->pt == cmp::pain_team::NONE;
	bool ignore_magnitude = cp->pp == cmp::pain_profile::NONE;
	return ignore_team || ignore_magnitude;
}

void pain_system::update(comm::msg_queue& msgs) {
	for(auto const& n : _nodes) {

		// Skip "don't care" nodes
		if(is_dont_care(n.cp)) continue;

		size_t hits = 0;

		n.coll_queue->for_each_report([this, &hits, &n, &msgs](cmp::coll_report const& r) {

			// Skip "don't care" collisions.
			if(is_dont_care(r.cp)) return;

			// Skip friendly fire.
			if(r.cp->pt == n.cp->pt) return;

			// Skip projectile - projectile collisions.
			if(r.cp->is_projectile && n.cp->is_projectile) return;

			// Record hit.
			++hits;

			// Compute and deal the pain.
			double pain = r.cp->dmg->compute_pain(r.cp->pp);
			n.wellness->deal_dmg(pain, r.origin_id);

			// Handle the pain flash
			if(pain > 0.0) *(n.pain_flash) = cfg::real("gfx_pain_flash_timer");
		});

		// Destroy if paper.
		if(n.cp->pp == cmp::pain_profile::PAPER && hits) {
			msgs.push(comm::create_remove_entity(n.id));
		}
	}

}

}
