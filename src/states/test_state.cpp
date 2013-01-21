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

#include <functional>
using std::bind;

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

#include "state.h"
#include "../misc/random_clock.h"
#include "../misc/rand.h"
#include "../cmp/systems.h"
#include "../cmp/comm.h"
#include "../cmp/entity_factory.h"

#include <allegro5/allegro_primitives.h>

// TODO:
// - Enemies flash when hit.

class test_state : public state {

	// External dependencies.
	// ----------------------
	config const& _config;
	resman const& _resman;

	// State.
	// ------
	map<int, bool> _keys;
	bool _done;
	uint64_t _player_id;
	vector<comm::message> _messages;

	// Generation processes.
	// ---------------------
	random_clock<uniform_real_distribution<double>> _eye_spawn_clk;
	random_clock<uniform_real_distribution<double>> _bomber_spawn_clk;

	// Systems.
	// --------
	sys::movement_system	_movement_system;
	sys::collision_system	_collision_system;
	sys::arms_system	_arms_system;
	sys::pain_system	_pain_system;
	sys::wellness_system	_wellness_system;
	sys::fx_system		_fx_system;
	sys::drawing_system	_drawing_system;
	sys::score_system	_score_system;
	sys::pickup_system	_pickup_system;

	// Factories.
	// ----------
	entity_factory _ef;
	uint64_t _last_id;

	// Message handling.
	// -----------------

	void handle_message(comm::message const& msg) {
		// TODO: Consider passing the message structs instead of the
		// tedious unpacking of the separate arguments...
		uint64_t id;
		switch(msg.type) {
		case comm::msg_t::remove_entity:
			id = msg.remove_entity.id;
			sys::remove_node(_movement_system, id);
			sys::remove_node(_collision_system, id);
			sys::remove_node(_arms_system, id);
			sys::remove_node(_pain_system, id);
			sys::remove_node(_wellness_system, id);
			sys::remove_node(_fx_system, id);
			sys::remove_node(_drawing_system, id);
			sys::remove_node(_score_system, id);
			sys::remove_node(_pickup_system, id);
			break;

		case comm::msg_t::spawn_bullet:
			_ef.create_bullet(msg.spawn_bullet.x,
					msg.spawn_bullet.y,
					msg.spawn_bullet.theta,
					msg.spawn_bullet.vx,
					msg.spawn_bullet.vy,
					msg.spawn_bullet.enemy,
					msg.spawn_bullet.origin_id);
			break;

		case comm::msg_t::spawn_missile:
			_ef.create_missile(msg.spawn_missile.x,
					msg.spawn_missile.y,
					msg.spawn_missile.theta,
					msg.spawn_missile.vx,
					msg.spawn_missile.vy,
					msg.spawn_missile.enemy,
					msg.spawn_missile.origin_id);
			break;

		case comm::msg_t::spawn_explosion:
			_ef.create_explosion(msg.spawn_explosion.x,
					msg.spawn_explosion.y);
			break;

		case comm::msg_t::spawn_smoke:
			_ef.create_smoke(msg.spawn_smoke.x,
				     msg.spawn_smoke.y,
				     msg.spawn_smoke.size);
			break;

		case comm::msg_t::spawn_debris:
			_ef.create_debris(msg.spawn_debris.x,
					msg.spawn_debris.y,
					msg.spawn_debris.vx,
					msg.spawn_debris.vy);
			break;

		case comm::msg_t::spawn_health_pickup:
			_ef.create_health_pickup(msg.spawn_health_pickup.x,
					msg.spawn_health_pickup.y,
					msg.spawn_health_pickup.vx,
					msg.spawn_health_pickup.vy);
			break;
			
		default:
			break;
		}
	}

	// GUI drawing.
	// ------------
	ALLEGRO_COLOR health_color(double health_ratio) {
		double r, g;
		if(health_ratio >= 0.5) {
			g = 1.0;
			r = 1.0 - (health_ratio - 0.5) * 2.0;
		} else {
			r = 1.0;
			g = health_ratio * 2.0;
		}
		return al_map_rgb_f(r, g, 0.0);
	}

	void draw_bar(double from_bottom, double ratio, ALLEGRO_COLOR color) {
		double x1 = _config.get_screen_w() * 0.05;
		double x2 = _config.get_screen_w() * 0.95;
		double y1 = _config.get_screen_h() - from_bottom - 5.0f;
		double y2 = _config.get_screen_h() - from_bottom;
		double hx2 = x1 + ratio * (x2 - x1);
		al_draw_filled_rectangle(x1, y1, hx2, y2, color);
	}	

	void draw_hud() {
		// Score.
		int player_score = int(_score_system.get_score(_player_id));
		al_draw_textf(
			_resman.get_font(res_id::FONT),
			al_map_rgba_f(1.0f, 1.0f, 1.0f, 1.0f),
			10.0f, 10.0f, 0,
			"Score: %d", player_score);
	
		// Health 
		double health_ratio = _wellness_system.
			get_ent_health_ratio(_player_id);

		draw_bar(10.0, health_ratio, health_color(health_ratio));
	}

public:
	test_state(const config& config, const resman& resman)
	: _config(config)
	, _resman(resman)
	, _done(false)
	, _eye_spawn_clk(
		uniform_real_distribution<double>(1.0, 2.0),
		bind(&entity_factory::create_eye, &_ef))
	, _bomber_spawn_clk(
		uniform_real_distribution<double>(5.0, 7.0),
		bind(&entity_factory::create_bomber, &_ef))
	, _drawing_system(resman.get_font(res_id::TINY_FONT))
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
		_pickup_system)
	, _last_id(0)
	{
		_keys[ALLEGRO_KEY_UP] = false;
		_keys[ALLEGRO_KEY_DOWN] = false;
		_keys[ALLEGRO_KEY_LEFT] = false;
		_keys[ALLEGRO_KEY_RIGHT] = false;
		_keys[ALLEGRO_KEY_LCTRL] = false;
		_player_id = _ef.create_player_ship(200.0, 200.0);
	}

	void sigkill() { _done = true; }
	bool done() { return _done; }
	unique_ptr<state> next_state() { return unique_ptr<state>(); }

	void frame_logic(double dt) {

		// Trigger the clocks.
		_eye_spawn_clk.tick(dt);
		_bomber_spawn_clk.tick(dt);

		// Update the player's throttle.
		double player_throttle_x = 0.0;
	       	double player_throttle_y = 0.0;
		if(_keys[ALLEGRO_KEY_RIGHT]) player_throttle_x += 1.0;
		if(_keys[ALLEGRO_KEY_LEFT]) player_throttle_x -= 1.0;
		if(_keys[ALLEGRO_KEY_DOWN]) player_throttle_y += 1.0;
		if(_keys[ALLEGRO_KEY_UP]) player_throttle_y -= 1.0;
		_movement_system.set_player_throttle(player_throttle_x, player_throttle_y);

		// Update the player's trigger.
		bool player_trigger = _keys[ALLEGRO_KEY_LCTRL];
		_arms_system.set_player_trigger(player_trigger);

		// Manage the debug ouptut. 
		_movement_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_collision_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_arms_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_pain_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_wellness_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_fx_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_drawing_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_score_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_pickup_system.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);

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

		// Hacky hud pass...
		draw_hud();

		// Handle messages.
		while(!_messages.empty()) {
			handle_message(_messages.back());
			_messages.pop_back();
		}
	}

	void key_up(int k) {
		if(k == ALLEGRO_KEY_ESCAPE)
			_done = true;
		_keys[k]= false;
	}

	void key_down(int k) {
		_keys[k]= true;
	}
};

unique_ptr<state> create_test_state(const config& config, const resman& res) {
	return unique_ptr<state>(new test_state(config, res));
}
