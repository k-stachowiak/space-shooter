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

#include "systems.h"
#include "../misc/rand.h"

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

	void drawing_system::update(double dt) {
		al_clear_to_color(al_map_rgb_f(0.0f, 0.0f, 0.0f));

		ALLEGRO_BITMAP* bmp;
		double x, y;
		double phi;
		for(auto const& n : _nodes) {
			n.appearance->update(dt);
			bmp = n.appearance->bitmap();
			x = n.orientation->get_x();
			y = n.orientation->get_y();
			phi = n.orientation->get_phi();
			int w = al_get_bitmap_width(bmp);
			int h = al_get_bitmap_height(bmp);
			al_draw_rotated_bitmap(bmp, 
					w >> 1, h >> 1,
					x, y, phi, 0);

			if(_debug_mode && n.shape) {
				n.shape->debug_draw();
				double cur_x = x + 10.0;
				double cur_y = y + 10.0;
				auto color = al_map_rgb_f(1.0f, 1.0f, 1.0f);
				for(auto const& d : n.dynamics) {
					al_draw_textf(
						_debug_font,
						color,
						cur_x, cur_y,
						0,
						"(%.1f, %.1f, %.1f),",
							d->get_vx(),
							d->get_vy(),
							d->get_theta());
					cur_y += 20.0;
				}
			}
		}
	}

	// FX system.
	// ----------
	
	void fx_system::update(
			double dt,
			vector<comm::message>& msgs) {
		double max_health;
		double health;
		double x, y;
		for(auto const& n : _nodes) {
			x = n.orientation->get_x();
			y = n.orientation->get_y();
			max_health = n.wellness->get_max_health();
			health = n.wellness->get_health();
			for(auto const& f : n.effects) {
				f->update(dt, health / max_health,
						x, y, msgs);
			}
		}
	}

	// Movement system.
	// ----------------
	
	void movement_system::update(
			double dt,
			vector<comm::message>& msgs) {

		for(auto const& n : _nodes) {

			// Determine velocities.
			// ---------------------
			double vx = 0, vy = 0;
			double theta = 0;
			
			if(n.id == _player_controlled) {
				vx = _player_throttle_x * 400.0;
				vy = _player_throttle_y * 300.0;

			} else {
				for(auto const& d : n.dynamics) { 
					d->update(dt);
					vx += d->get_vx();
					vy += d->get_vy();
					theta += d->get_theta();
				}
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
					msgs.push_back(comm::create_remove_entity(
								n.id));
					continue;
				}
				mini = n.life_bounds->get_x_min();
				maxi = n.life_bounds->get_y_max();
				if(!between(y + dy, mini, maxi)) {
					msgs.push_back(comm::create_remove_entity(
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
			double dt,
			vector<comm::message>& msgs,
			double x,
			double y) {

		if(!_player_trigger) {
			_player_counter = 0.0;
			return;
		}

		_player_counter -= dt;
		if(_player_counter > 0.0)
			return;

		_player_counter += _player_interval;
		if(_player_prev_left) {
			_player_prev_left = false;
			msgs.push_back(comm::create_spawn_bullet(
						x + 15.0, y,
						-1.57, 0.0,
						-800.0,
						false,
						id));
		} else {
			_player_prev_left = true;
			msgs.push_back(comm::create_spawn_bullet(
						x - 15.0, y,
						-1.57, 0.0,
						-800.0,
						false,
						id));
		}
	}

	void arms_system::update(double dt, vector<comm::message>& msgs) {
		double x, y;
		for(auto const& n : _nodes) {
			x = n.orientation->get_x();
			y = n.orientation->get_y();

			if(n.id == _player_shooting) {
				handle_player(n.id, dt, msgs, x, y);
				continue;
			}

			for(auto const& wb : n.weapon_beh)
				wb->update(n.id, dt, x, y, msgs);
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
	
	void pain_system::update(vector<comm::message>& msgs) {
		for(auto const& n : _nodes) {
			n.coll_queue->for_each_report([&n, &msgs](cmp::coll_report const& r) {
				
				// Determine which of the colliding entities
				// is the "other one".
				cmp::coll_class other_cc;
				uint64_t other_origin_id;

				// Note that the real colliding id's are compared here,
				// but the originating id is recorded for further use.
				if(r.a.id == n.id) {
					other_cc = r.b.cc;
					other_origin_id = r.b.origin_id;
				} else {
					other_cc = r.a.cc;
					other_origin_id = r.a.origin_id;
				}

				// Determine the damage amount and deal it to the "other one".
				double pain = n.painmap->get_pain(other_cc);
				n.wellness->deal_dmg(pain, other_origin_id);
			});
		}
	}

	// Wellness system.
	// ----------------
	
	void wellness_system::update(double dt, vector<comm::message>& msgs) {
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
				if(n.explodes)
					msgs.push_back(comm::create_spawn_explosion(
								n.orientation->get_x(),
								n.orientation->get_y()));
				double vx = 0;
				double vy = 0;
				for(auto const& d : n.dynamics) {
					vx += d->get_vx();
					vy += d->get_vy();
				}

				for(uint32_t i = 0; i < n.num_debris; ++i) {
					msgs.push_back(comm::create_spawn_debris(
								n.orientation->get_x(),
								n.orientation->get_y(),
								vx, vy));
				}
				
				msgs.push_back(comm::create_remove_entity(n.id));
			}
		}
	}
}
