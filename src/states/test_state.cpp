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

#include <functional>
using std::bind;

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

#include "state.h"
#include "../misc/random_clock.h"
#include "../misc/rand.h"
#include "../efwk/systems.h"
#include "../efwk/comm.h"
#include "../efwk/entity_factory.h"

#include <allegro5/allegro_primitives.h>

// TODO:
// - Consider "tracked entities" once the hud system works.
// - Large ship pieces
// - waves and patterns
// - Dry-run the stars generator so that the screen starts filled with some initial stars.

class test_state : public state {

	// External dependencies.
	// ----------------------
	config const& _config;
	resman const& _resman;

	// State.
	// ------
	bool _debug;
	bool _done;
	uint64_t _player_id;
	comm::msg_queue _messages;

	// Generation processes.
	// ---------------------
	random_clock<uniform_real_distribution<double>> _star_spawn_clk;
	random_clock<uniform_real_distribution<double>> _l_fighter_spawn_clk;
	random_clock<uniform_real_distribution<double>> _h_fighter_spawn_clk;
	random_clock<uniform_real_distribution<double>> _l_bomber_spawn_clk;
	random_clock<uniform_real_distribution<double>> _h_bomber_spawn_clk;

	// Systems.
	// --------
	sys::movement_system	_movement_system;
	sys::collision_system	_collision_system;
	sys::arms_system		_arms_system;
	sys::pain_system		_pain_system;
	sys::wellness_system	_wellness_system;
	sys::fx_system			_fx_system;
	sys::drawing_system		_drawing_system;
	sys::score_system		_score_system;
	sys::pickup_system		_pickup_system;
	sys::input_system		_input_system;
	sys::hud_system			_hud_system;

	// Factories.
	// ----------
	entity_factory _ef;

	// Message handling.
	// -----------------

	void handle_messages(double dt) {

		_messages.for_each_msg(dt, [this](comm::message const& msg) {

			uint64_t id;
			switch(msg.type) {
			case comm::msg_t::remove_entity:
				id = msg.remove_entity.id;
				remove_node(_movement_system, id);
				remove_node(_collision_system, id);
				remove_node(_arms_system, id);
				remove_node(_pain_system, id);
				remove_node(_wellness_system, id);
				remove_node(_fx_system, id);
				remove_node(_drawing_system, id);
				remove_node(_score_system, id);
				remove_node(_pickup_system, id);
				remove_node(_input_system, id);
				remove_node(_hud_system, id);
				break;

			case comm::msg_t::spawn_bullet:
				_ef.create_bullet(
						msg.spawn_bullet.x,
						msg.spawn_bullet.y,
						msg.spawn_bullet.theta,
						msg.spawn_bullet.vx,
						msg.spawn_bullet.vy,
						msg.spawn_bullet.upgrade_lvl,
						msg.spawn_bullet.enemy,
						msg.spawn_bullet.origin_id);
				break;

			case comm::msg_t::spawn_missile:
				_ef.create_missile(
						msg.spawn_missile.x,
						msg.spawn_missile.y,
						msg.spawn_missile.theta,
						msg.spawn_missile.vx,
						msg.spawn_missile.vy,
						msg.spawn_missile.upgrade_lvl,
						msg.spawn_missile.enemy,
						msg.spawn_missile.origin_id);
				break;

			case comm::msg_t::spawn_explosion:
				_ef.create_explosion(
						msg.spawn_explosion.x,
						msg.spawn_explosion.y);
				break;

			case comm::msg_t::spawn_smoke:
				_ef.create_smoke(
						msg.spawn_smoke.x,
					    msg.spawn_smoke.y,
					    msg.spawn_smoke.size);
				break;

			case comm::msg_t::spawn_debris:
				_ef.create_debris(
						msg.spawn_debris.x,
						msg.spawn_debris.y,
						msg.spawn_debris.vx,
						msg.spawn_debris.vy,
						msg.spawn_debris.vmin,
						msg.spawn_debris.vmax,
						msg.spawn_debris.theta_min,
						msg.spawn_debris.theta_max,
						msg.spawn_debris.image,
						msg.spawn_debris.explode,
						msg.spawn_debris.origin_id);
				break;

			case comm::msg_t::spawn_health_pickup:
				_ef.create_health_pickup(
						msg.spawn_health_pickup.x,
						msg.spawn_health_pickup.y,
						msg.spawn_health_pickup.vx,
						msg.spawn_health_pickup.vy);
				break;

			case comm::msg_t::spawn_battery_pickup:
				_ef.create_battery_pickup(
						msg.spawn_battery_pickup.x,
						msg.spawn_battery_pickup.y,
						msg.spawn_battery_pickup.vx,
						msg.spawn_battery_pickup.vy);
				break;

			case comm::msg_t::spawn_missiles_pickup:
				_ef.create_missiles_pickup(
						msg.spawn_missiles_pickup.x,
						msg.spawn_missiles_pickup.y,
						msg.spawn_missiles_pickup.vx,
						msg.spawn_missiles_pickup.vy);
				break;
				
			case comm::msg_t::spawn_bullet_upgrade_pickup:
				_ef.create_bullet_upgrade_pickup(
						msg.spawn_bullet_upgrade_pickup.x,
						msg.spawn_bullet_upgrade_pickup.y,
						msg.spawn_bullet_upgrade_pickup.vx,
						msg.spawn_bullet_upgrade_pickup.vy);
				break;

			case comm::msg_t::spawn_missile_upgrade_pickup:
				_ef.create_missile_upgrade_pickup(
						msg.spawn_missile_upgrade_pickup.x,
						msg.spawn_missile_upgrade_pickup.y,
						msg.spawn_missile_upgrade_pickup.vx,
						msg.spawn_missile_upgrade_pickup.vy);
				break;

			default:
				break;
			}
		});
	}
	
	// This seems necessary for the clock declarations...
	using uni_distr = uniform_real_distribution<double>;

public:
	test_state(const config& config, const resman& resman)
	: _config(config)
	, _resman(resman)
	, _debug(false)
	, _done(false)
	, _star_spawn_clk(uni_distr(0.125, 0.25), bind(&entity_factory::create_star, &_ef))
	, _l_fighter_spawn_clk(uni_distr(2.0, 4.0), bind(&entity_factory::create_light_fighter, &_ef))
	, _h_fighter_spawn_clk(uni_distr(4.0, 6.0), bind(&entity_factory::create_heavy_fighter, &_ef))
	, _l_bomber_spawn_clk(uni_distr(6.0, 8.0), bind(&entity_factory::create_light_bomber, &_ef))
	, _h_bomber_spawn_clk(uni_distr(8.0, 10.0), bind(&entity_factory::create_heavy_bomber, &_ef))
	, _drawing_system(resman.get_font(res_id::TINY_FONT))
	, _hud_system(
			_resman.get_bitmap(res_id::HUD_BG),
			_resman.get_bitmap(res_id::HEALTH),
			_resman.get_bitmap(res_id::BATTERY),
			_resman.get_bitmap(res_id::MISSILES),
			_resman.get_bitmap(res_id::DIODE_ON),
			_resman.get_bitmap(res_id::DIODE_OFF),
			_resman.get_bitmap(res_id::B_UPGRADE),
			_resman.get_bitmap(res_id::M_UPGRADE),
			_resman.get_font(res_id::FONT),
			_resman.get_font(res_id::TINY_FONT),
			_config.get_screen_w(),
			_config.get_screen_h())
	, _ef(_config,
		_resman,
		_movement_system,
		_collision_system,
		_arms_system,
		_pain_system,
		_wellness_system,
		_fx_system,
		_drawing_system,
		_score_system,
		_pickup_system,
		_input_system,
		_hud_system)
	{
		_player_id = _ef.create_player_ship(200.0, 200.0);
	}

	void sigkill() { _done = true; }
	bool done() { return _done; }
	unique_ptr<state> next_state() { return unique_ptr<state>(); }

	void frame_logic(double dt) {

		// Trigger the clocks.
		_star_spawn_clk.tick(dt);
		_l_fighter_spawn_clk.tick(dt);
		_h_fighter_spawn_clk.tick(dt);
		_l_bomber_spawn_clk.tick(dt);
		_h_bomber_spawn_clk.tick(dt);

		// Manage the debug ouptut. 
		_movement_system.set_debug_mode(_debug);
		_collision_system.set_debug_mode(_debug);
		_arms_system.set_debug_mode(_debug);
		_pain_system.set_debug_mode(_debug);
		_wellness_system.set_debug_mode(_debug);
		_fx_system.set_debug_mode(_debug);
		_drawing_system.set_debug_mode(_debug);
		_score_system.set_debug_mode(_debug);
		_pickup_system.set_debug_mode(_debug);
		_input_system.set_debug_mode(_debug);
		_hud_system.set_debug_mode(_debug);

		// Update the systems.
		_movement_system.update(dt, _messages);
		_collision_system.update();
		_arms_system.update(dt, _messages);
		_pain_system.update(_messages);
		_wellness_system.update(dt, _messages);
		_fx_system.update(dt, _messages);
		_drawing_system.update(dt);
		_score_system.update();
		_pickup_system.update(_messages); 
		_input_system.update();
		_hud_system.update();

		// Handle messages.
		handle_messages(dt);
	}

	void key_up(int k) {
		if(k == ALLEGRO_KEY_ESCAPE) _done = true;
		if(k == ALLEGRO_KEY_SPACE) _debug = true;
		_input_system.key_up(k);
	}

	void key_down(int k) {
		if(k == ALLEGRO_KEY_SPACE) _debug = false;
		_input_system.key_down(k);
	}
};

unique_ptr<state> create_test_state(const config& config, const resman& res) {
	return unique_ptr<state>(new test_state(config, res));
}
