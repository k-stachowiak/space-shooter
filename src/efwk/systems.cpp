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

static inline void resolve_coll_report(
		cmp::coll_report const& report,
		uint64_t this_id,
		cmp::coll_class& other_cc,
		uint64_t& other_id,
		uint64_t& other_origin_id) {

	// Note that the real colliding id's are compared here,
	// but the originating id is recorded for further use.
	if(report.a.id == this_id) {
		other_cc = report.b.cc;
		other_id = report.b.id;
		other_origin_id = report.b.origin_id;
	} else {
		other_cc = report.a.cc;
		other_id = report.a.id;
		other_origin_id = report.a.origin_id;
	}
}

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
			double vx = 0, vy = 0;
			double theta = 0;
			
			if(n.id == _player.id) {
				vx = _player.throttle_x * 400.0;
				vy = _player.throttle_y * 300.0;

			} else {
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
	
	void arms_system::handle_player(uint64_t id,
			shared_ptr<cmp::ammo> ammo,
			double dt,
			comm::msg_queue& msgs,
			double x,
			double y) {
		
		// Store the ammo info for the core logic.
		// ---------------------------------------
		_player.bullets = ammo->get_bullets();
		_player.rockets = ammo->get_rockets();

		// Minigun fire.
		// -------------
		if(_player.minigun.update(dt) && ammo->get_bullets() != 0) {
			ammo->add_bullets(-1);
			if(_player.prev_left) {
				_player.prev_left = false;
				msgs.push(comm::create_spawn_bullet(
							x + 15.0, y,
							-1.57, 0.0,
							-800.0,
							false,
							id));
			} else {
				_player.prev_left = true;
				msgs.push(comm::create_spawn_bullet(
							x - 15.0, y,
							-1.57, 0.0,
							-800.0,
							false,
							id));
			}
		}

		// Rocket launcher fire.
		// ---------------------
		if(_player.rpg.update(dt) && ammo->get_rockets() != 0) {
			ammo->add_rockets(-1);
			msgs.push(comm::create_spawn_missile(
						x + 25.0, y,
						-1.57, 0.0,
						-300.0,
						false,
						id));
			msgs.push(comm::create_spawn_missile(
						x - 25.0, y,
						-1.57, 0.0,
						-300.0,
						false,
						id));
		}
	}

	void arms_system::update(double dt, comm::msg_queue& msgs) {
		double x, y;
		for(auto const& n : _nodes) {
			x = n.orientation->get_x();
			y = n.orientation->get_y();

			if(n.id == _player.id) {
				handle_player(n.id, n.ammo, dt, msgs, x, y);
				continue;
			}

			if(n.weapon_beh)
				n.weapon_beh->update(n.id, n.ammo, dt, x, y, msgs);
		}
	}

	// Collisions system.
	// ------------------

	void collision_system::check_collision(nd::collision_node const& a,
			     nd::collision_node const& b) {

		cmp::shape const& shp_a = *(a.shape);
		cmp::shape const& shp_b = *(b.shape);

		if(shp_a.collides_with(shp_b)) {
			cmp::coll_report report {
				{ a.id, a.origin_id, a.cc, a.shape },
				{ b.id, b.origin_id, b.cc, b.shape }
			};
			a.coll_queue->push_report(report);
			b.coll_queue->push_report(report);
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

	double pain_system::compute_multiplier(cmp::coll_class cc, uint64_t origin_id) const {

		if(_upgrades_map.find(origin_id) == end(_upgrades_map)) {
			return 1.0;
		}

		// Determine the upgrades of the offender.
		auto const& upgrades = _upgrades_map.at(origin_id);

		// Decide, which of the upgrades to take into account
		// based on what has hit you.
		size_t upgrade_lvl;
		switch(cc) {
		case cmp::coll_class::PLAYER_BULLET:
		case cmp::coll_class::ENEMY_BULLET:
			upgrade_lvl = upgrades.gun_lvl();
			break;

		case cmp::coll_class::PLAYER_MISSILE:
	 	case cmp::coll_class::ENEMY_MISSILE:
			upgrade_lvl = upgrades.rl_lvl();
			break;

		default:
			throw;
		}

		return upgrade_lvl;
	}
	
	void pain_system::update(comm::msg_queue& msgs) {
		for(auto const& n : _nodes) {
			// Store the upgrades info.
			if(n.upgrades) 
				_upgrades_map[n.id] = *(n.upgrades);

			// Analyze collisions.
			n.coll_queue->for_each_report([this, &n, &msgs](cmp::coll_report const& r) {
				
				// Determine which of the colliding entities
				// is the "other one".
				cmp::coll_class other_cc;
				uint64_t other_id;
				uint64_t other_origin_id;
				resolve_coll_report(r, n.id, other_cc, other_id, other_origin_id);

				// Determine the damage amount and the damage multiplier.
				double pain = n.painmap->get_pain(other_cc);
				double multiplier = compute_multiplier(other_cc, other_origin_id);

				n.wellness->deal_dmg(pain * multiplier, other_origin_id);

				if(pain > 0.0) {
					*(n.pain_flash) = 0.025;
				}
			});
		}
	}

	// Pickup system.
	// --------------
	
	void pickup_system::update(comm::msg_queue& msgs) {
		for(auto const& n : _nodes) {
			n.coll_queue->for_each_report([&n, &msgs](cmp::coll_report const& r) {

				// Determine which of the colliding entities
				// is the "other one".
				cmp::coll_class other_cc;
				uint64_t other_id;
				uint64_t other_origin_id;
				resolve_coll_report(r, n.id, other_cc, other_id, other_origin_id);

				double h = n.wellness->get_health();
				double H = n.wellness->get_max_health();
				double dh = 10;

				bool picked_up = false;
				switch(other_cc) {
				case cmp::coll_class::HEALTH_PICKUP:
					if(h < H) {
						picked_up = true;
						if(h + dh >= H)
							n.wellness->add_health(H - h);
						else
							n.wellness->add_health(dh);
					}
					break;

				case cmp::coll_class::MISSILES_PICKUP:
					n.ammo->add_rockets(7);
					picked_up = true;
					break;

				default:
					break;
				}

				if(picked_up) {
					msgs.push(comm::create_remove_entity(other_id));
				}
			});
		}
	}

	// Wellness system.
	// ----------------
	
	void wellness_system::update(double dt, comm::msg_queue& msgs) {
		for(auto const& n : _nodes) {

			if(n.wellness) {
				_entity_health_map[n.id] = n.wellness->get_health();
				_entity_max_health_map[n.id] = n.wellness->get_max_health();
			}

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
		}
	}
}
