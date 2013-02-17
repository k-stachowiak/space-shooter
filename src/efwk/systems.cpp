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
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <random>
using std::uniform_real_distribution;
using std::bernoulli_distribution;
using std::exponential_distribution;

#include "systems.h"
#include "../misc/rand.h"

// Common functions.
// -----------------

template<class T>
inline bool between(T value, T min, T max) { return value >= min && value <= max; }

// Systems implementations.
// ------------------------

namespace sys {

	// Score system.
	// -------------

	void score_system::update() {
		for(auto const& n : _nodes) {
			
			if(n.wellness->is_alive()) {
				continue;
			}

			uint64_t receiver = n.wellness->get_last_dmg_id();
			double score = _class_score_map[n.sc];
			_ent_score_map[receiver] += score;
		}
	}

	// Input system.
	// -------------
	
	void input_system::update() {
		for(auto const& n : _nodes) {
			n.dynamics->input(_keys);
			n.weapon_beh->input(_keys);
		}
	}

	// Drawing system.
	// ---------------

	void drawing_system::draw_plane(double dt, vector<nd::drawing_node> const& nodes) {

		double x, y;
		double phi;
		for(auto const& n : nodes) {

			n.appearance->update(dt);

			x = n.orientation->get_x();
			y = n.orientation->get_y();
			phi = n.orientation->get_phi();

			if(*(n.pain_flash) > 0.0) {
				n.appearance->draw_flash(x, y, phi);
				*(n.pain_flash) -= dt;
			} else {
				n.appearance->draw(x, y, phi);
			}

			if(_debug_mode && n.shape) {
				n.shape->debug_draw();
				if(n.dynamics) {
					al_draw_textf(
						_debug_font,
						al_map_rgb_f(1.0f, 1.0f, 1.0f),
						x + 10, y + 10,
						0,
						"(%.1f, %.1f, %.1f),",
							n.dynamics->get_vx(),
							n.dynamics->get_vy(),
							n.dynamics->get_theta());
				}
			}
		}
	}

	void drawing_system::update(double dt) {
		al_clear_to_color(al_map_rgb_f(0.0f, 0.0f, 0.0f));
		draw_plane(dt, _nodes[cmp::draw_plane::BACKGROUND]);
		draw_plane(dt, _nodes[cmp::draw_plane::SHIPS]);
		draw_plane(dt, _nodes[cmp::draw_plane::PROJECTILES]);
		draw_plane(dt, _nodes[cmp::draw_plane::FX]);
	}

	// FX system.
	// ----------
	
	void fx_system::update(
			double dt,
			comm::msg_queue& msgs) {
		double max_health;
		double health;
		double x, y;
		for(auto const& n : _nodes) {
			x = n.orientation->get_x();
			y = n.orientation->get_y();
			max_health = n.wellness->get_max_health();
			health = n.wellness->get_health();
			n.effects->update(dt,
					health / max_health,
					x, y,
					n.shape,
					msgs);
		}
	}

	// Movement system.
	// ----------------
	
	void movement_system::update(
			double dt,
			comm::msg_queue& msgs) {

		for(auto const& n : _nodes) {

			// Determine velocities.
			// ---------------------
			double vx = 0;
			double vy = 0;
			double theta = 0;

			if(n.dynamics) {
				n.dynamics->update(dt);
				vx = n.dynamics->get_vx();
				vy = n.dynamics->get_vy();
				theta = n.dynamics->get_theta();
			}

			// Check bounds.
			// -------------
			double x = n.orientation->get_x();
			double y = n.orientation->get_y();
			double phi = n.orientation->get_phi();

			double dx = vx * dt;
			double dy = vy * dt;
			double dphi = theta * dt;

			double mini, maxi;

			if(n.movement_bounds) {
				mini = n.movement_bounds->get_x_min();
				maxi = n.movement_bounds->get_x_max();
				if(!between((x + dx), mini, maxi))
					dx = 0.0;

				mini = n.movement_bounds->get_y_min();
				maxi = n.movement_bounds->get_y_max();
				if(!between((y + dy), mini, maxi))
					dy = 0.0;

			}

			if(n.life_bounds) {
				mini = n.life_bounds->get_x_min();
				maxi = n.life_bounds->get_x_max();
				if(!between(x + dx, mini, maxi)) {
					msgs.push(comm::create_remove_entity(
								n.id));
					continue;
				}
				mini = n.life_bounds->get_x_min();
				maxi = n.life_bounds->get_y_max();
				if(!between(y + dy, mini, maxi)) {
					msgs.push(comm::create_remove_entity(
								n.id));
					continue;
				}
			}

			// Perform the actual move.
			// ------------------------
			n.orientation->set_x(x + dx);
			n.orientation->set_y(y + dy);
			n.orientation->set_phi(phi + dphi);

			if(n.shape) {
				n.shape->shift(dx, dy);
				n.shape->rotate(dphi);
			}
		}
	}

	// Arms system.
	// ------------
	
	void arms_system::update(double dt, comm::msg_queue& msgs) {
		double x, y;
		_tracked_ammo.reset();
		for(auto const& n : _nodes) {

			x = n.orientation->get_x();
			y = n.orientation->get_y();

			if(n.weapon_beh)
				n.weapon_beh->update(n.id, n.ammo, dt, x, y, msgs);

			if(n.id == _tracked_id)
				_tracked_ammo = n.ammo;
		}
	}

	// Collisions system.
	// ------------------

	void collision_system::check_collision(nd::collision_node const& a,
			     nd::collision_node const& b) {

		cmp::shape const& shp_a = *(a.shape);
		cmp::shape const& shp_b = *(b.shape);

		if(shp_a.collides_with(shp_b)) {
			a.coll_queue->push_report({ b.id, b.origin_id, b.cp, b.shape });
			b.coll_queue->push_report({ a.id, a.origin_id, a.cp, a.shape });
		}
	}

	void collision_system::update() {
		// Clear the collision queues first.
		for(auto a = begin(_nodes); a != end(_nodes); ++a)
			a->coll_queue->clear();

		// Perform the collision checks.
		for(auto a = begin(_nodes); a != end(_nodes); ++a)
			for(auto b = a + 1; b != end(_nodes); ++b)
				check_collision(*a, *b);
	}

	// Pain system.
	// ------------
	
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

				// Record hit.
				++hits;

				// Compute and deal the pain.
				double pain = n.cp->dmg->compute_pain(r.cp->pp);
				n.wellness->deal_dmg(pain, r.origin_id);

				// Handle the pain flash
				if(pain > 0.0) *(n.pain_flash) = 0.025;
			});

			// Destroy if paper.
			if(n.cp->pp == cmp::pain_profile::PAPER && hits) {
				msgs.push(comm::create_remove_entity(n.id));
			}
		}

	}

	// Pickup system.
	// --------------
	
	void pickup_system::update(comm::msg_queue& msgs) {
		for(auto const& n : _nodes) {
			n.coll_queue->for_each_report([&n, &msgs](cmp::coll_report const& r) {
				if(r.cp->pickup) {
					bool picked_up = r.cp->pickup->trigger(n.wellness, n.ammo);
					if(picked_up) {
						msgs.push(comm::create_remove_entity(r.id));
					}
				}
			});
		}
	}

	// Wellness system.
	// ----------------
	
	void wellness_system::update(double dt, comm::msg_queue& msgs) {

		_tracked_wellness.reset();

		for(auto const& n : _nodes) {

			bool died = false;
			if(n.wellness) {
				if(!n.wellness->is_alive())
					died = true;
			}

			if(n.ttl) {
				n.ttl->update(dt);
				if(n.ttl->get_ticks() > 0)
					died = true;
			}

			if(died) {
				double vx = 0;
				double vy = 0;
				if(n.dynamics) {
					// Read the velocity.
					vx = n.dynamics->get_vx();
					vy = n.dynamics->get_vy();
				}

				// Handle reactions.
				if(n.on_death)
					n.on_death->trigger(
						n.orientation->get_x(),
						n.orientation->get_y(),
						n.orientation->get_phi(),
						n.shape,
						vx, vy,
						n.id,
						msgs);
				
				// Remove the entity.
				msgs.push(comm::create_remove_entity(n.id));
			}

			if(n.id == _tracked_id) {
				_tracked_wellness = n.wellness;
			}
		}
	}
}
