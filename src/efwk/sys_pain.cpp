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

void pain_system::update(comm::msg_queue& msgs) {

	for(auto const& n : _nodes) {

		cmp::coll_queue const& cq = *n.coll_queue;
		cq.for_each_report([this, &n, &msgs](cmp::coll_report const& r) {

			uint64_t id = n.id;
			cmp::wellness& w = *n.wellness;
			shared_ptr<double> pf = n.pain_flash;
			cmp::collision_profile const& this_cp = *n.cp;
			cmp::collision_profile const& other_cp = *r.cp;

			switch(this_cp.cc) {
			case cmp::coll_class::SHIP:
				if ((other_cp.cc == cmp::coll_class::SHIP ||
						other_cp.cc == cmp::coll_class::PROJECTILE) &&
						this_cp.pt != other_cp.pt) {

					w.deal_dmg(other_cp.dmg, r.origin_id);
					*(pf) = cfg::real("gfx_pain_flash_timer");
				}
				break;

			case cmp::coll_class::PROJECTILE:
				if(other_cp.cc == cmp::coll_class::SHIP && this_cp.pt != other_cp.pt)
					msgs.push(comm::create_remove_entity(id));
				break;

			case cmp::coll_class::PICKUP:
			default:
				break;
			}
		});
	}

}

}
