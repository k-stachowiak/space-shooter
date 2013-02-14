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

#include <iostream>
using std::cout;
using std::endl;

#include "../misc/rand.h"
#include "../geometry/bezier.h"
#include "entity_factory.h"

uint64_t entity_factory::create_explosion(double x, double y) {

	// Helpers.
	// --------
	uint32_t frame_width = 70;
	uint32_t num_frames = 16;
	double frame_time = 0.05;

	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	shared_ptr<cmp::dynamics> dynamics; 
	shared_ptr<cmp::shape> shape; 

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::FX;
	auto pain_flash = make_shared<double>(0.0);
	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(res_id::EXPLOSION),
			_resman.get_bitmap(res_id::EXPLOSION),
			frame_width,
			num_frames,
			frame_defs,
			1);

	// Wellness components.
	// --------------------
	auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 
	shared_ptr<cmp::reaction> on_death;
	shared_ptr<cmp::wellness> wellness; 

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });

	return id;
}

uint64_t entity_factory::create_smoke(double x, double y, comm::smoke_size size) {

	// Helpers.
	// --------

	// Interpret size.
	res_id rid;
	double scale;
	switch(size) {
		case comm::smoke_size::tiny:
			rid = res_id::SMOKE_SMALL;
			scale = 0.5;
			break;

		case comm::smoke_size::medium:
			rid = res_id::SMOKE;
			scale = 1.0;
			break;
			
		case comm::smoke_size::big:
			rid = res_id::SMOKE_BIG;
			scale = 2.0;
			break;

		default:
			throw;
	}

	// Prepare animation frame definitions.
	uint32_t frame_width = 32 * 3 * scale;
	uint32_t num_frames = 25;
	double frame_time = 0.075;
	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	
	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	auto pain_flash = make_shared<double>();
	shared_ptr<cmp::dynamics> dynamics; 
	shared_ptr<cmp::shape> shape; 

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::FX;
	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(rid),
			_resman.get_bitmap(rid),
			frame_width,
			num_frames,
			frame_defs,
			1);

	// Wellness components.
	// --------------------
	auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 
	shared_ptr<cmp::reaction> on_death;
	shared_ptr<cmp::wellness> wellness; 

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });

	return id;
}

uint64_t entity_factory::create_debris(double x, double y,
		double bvx, double bvy,
		double vmin, double vmax,
		double theta_min, double theta_max,
		res_id bmp,
		bool explode,
		uint64_t origin_id) {

	// Helpers.
	// --------
	double debris_health = 1;

	// TTL generation.
	uniform_real_distribution<double> ttl_dist(0.25, 1.0);
	double ttl_time = ttl_dist(rnd::engine);

	// Random movement.
	bernoulli_distribution dir_dist;
	
	uniform_real_distribution<double> mv_dist(vmin, vmax);
	double base_vx = mv_dist(rnd::engine);
	double base_vy = mv_dist(rnd::engine);
	double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
	double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

	uniform_real_distribution<double> rot_dist(theta_min, theta_max);
	double base_av = rot_dist(rnd::engine);
	double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	auto pain_flash = make_shared<double>(0.0);
	auto shape = cmp::create_circle(x, y, 8.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::FX;
	auto appearance = cmp::create_static_bmp(
		_resman.get_bitmap(bmp),
		_resman.get_bitmap(bmp));

	// Wellness nodes.
	// ---------------
	auto wellness = cmp::create_wellness(debris_health); 
	auto ttl = cmp::create_const_int_timer(ttl_time); 
	auto on_death = explode
		? cmp::create_explosion_sequence_reaction(1)
		: shared_ptr<cmp::reaction>();

	// Movement nodes.
	// ---------------
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 
	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(bvx + base_vx * mul_vx, bvy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(base_av * mul_av)
	});

	// Collision nodes.
	// ----------------
	auto coll_queue = cmp::create_coll_queue();
	auto cc = cmp::coll_class::DEBRIS;

	// Pain nodes.
	// -----------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::PLAYER_SHIP, debris_health },
		{ cmp::coll_class::ENEMY_SHIP, debris_health }
	});

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue }); 
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash }); 

	return id;
}

uint64_t entity_factory::create_star() {

	// Helpers.
	// --------
	uniform_real_distribution<double> uni_dist;
	double grade = uni_dist(rnd::engine);

	double vy = 200.0 * (grade * 0.5 + 0.5);
	double shade = grade * 0.9 + 0.1;
	
	double x = uni_dist(rnd::engine) * _config.get_screen_w();
	
	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, 1.0, 0.0);
	auto pain_flash = make_shared<double>(0.0);
	shared_ptr<cmp::shape> shape;

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::BACKGROUND;
	auto appearance = cmp::create_pixel(shade, shade, shade);

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_velocity_dynamics(0.0, vy);
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });

	return id;
}

uint64_t entity_factory::create_player_ship(double x, double y) {

	// Initialize components.
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, -1.57); 
	auto shape = cmp::create_circle(x, y, 32.0); 
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::SHIPS;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::PLAYER_SHIP),
			_resman.get_bitmap(res_id::PLAYER_SHIP_FLASH));

	// Movement components.
	// --------------------
	auto life_bounds = shared_ptr<cmp::bounds>(); 
	auto movement_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 
	shared_ptr<cmp::dynamics> dynamics; 

	// Arms components.
	// ----------------
	shared_ptr<cmp::weapon_beh> weapon_beh; // TODO: Have a "player contolled weapon behavior"
	auto ammo = cmp::create_ammo(-1, 3);

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue(); 
	auto cc = cmp::coll_class::PLAYER_SHIP; 

	// Pain components.
	// ----------------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::ENEMY_BULLET, 10.0 },
		{ cmp::coll_class::ENEMY_MISSILE, 20.0 },
		{ cmp::coll_class::ENEMY_SHIP, 25.0 },
		{ cmp::coll_class::DEBRIS, 10.0 }
	}); 

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(100.0); 
	auto on_death = cmp::create_complex_reaction({
		cmp::create_debris_reaction(10, {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 },
			100.0, 300.0,
			4.0, 7.0,
			/* explode = */ false,
			/* randomize = */ true),
		cmp::create_explosion_sequence_reaction(7)
	});

	shared_ptr<cmp::timer> ttl; 

	// Fx components.
	// --------------
	auto fxs = cmp::create_smoke_when_hurt(0.25);

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });
	_pickup_system.add_node({ id, coll_queue, wellness, ammo });

	return id;
}

uint64_t entity_factory::create_light_fighter() {

	// Helpers.
	// --------
	bernoulli_distribution xdir_dist;

	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	
	const double vx = dir * 75.0;
	const double vy = 75.0;

	const double offscreen = 30.0;
	const double x = (dir > 0.0) ? -offscreen : _config.get_screen_w() + offscreen;
	const double y = -offscreen;

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 1.57);
	auto shape = cmp::create_circle(x, y, 32.0);
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::SHIPS;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER),
			_resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER_FLASH)); 

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
	auto movement_bounds = shared_ptr<cmp::bounds>();
	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	// Arms components.
	// ----------------
	auto ammo = cmp::create_ammo_unlimited();
	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(3.0, 3.0, +17.0, -3.0),
			cmp::create_period_bullet(3.0, 3.0, -17.0, -3.0) });

	// Collision components.
	// ---------------------
	auto cc = cmp::coll_class::ENEMY_SHIP;
	auto coll_queue = cmp::create_coll_queue();

	// Pain components.
	// ----------------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::PLAYER_BULLET, 10.0 },
		{ cmp::coll_class::PLAYER_MISSILE, 30.0 },
		{ cmp::coll_class::PLAYER_SHIP, 50.0 },
		{ cmp::coll_class::DEBRIS, 10.0 }
	});

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(30.0); 
	vector<shared_ptr<cmp::reaction>> reactions {
		cmp::create_debris_reaction(3, {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 },
			100.0, 300.0,
			4.0, 7.0,
			/* explode = */ false,
			/* randomize = */ true),
		cmp::create_explosion_sequence_reaction(1) };

	bernoulli_distribution drop_health(0.1);
	if(drop_health(rnd::engine))
		reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.05);
	if(drop_missile(rnd::engine))
		reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);
	shared_ptr<cmp::timer> ttl;

	// Fx components.
	// --------------
	auto fxs = cmp::create_smoke_when_hurt(0.25);

	// Score components.
	// -----------------
	auto sc = cmp::score_class::ENEMY_LIGHT_FIGHTER;

	// Register components.
	// --------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_heavy_fighter() {

	// Helpers.
	// --------

	// Path points.
	uniform_real_distribution<double> x_margin_dist(50.0, 200.0);
	
	const double offscreen = 30.0;
	const double x_margin = x_margin_dist(rnd::engine);

	const double y0 = -offscreen;
	const double y1 = _config.get_screen_h() * 0.5;
	const double y2 = _config.get_screen_h() + offscreen;

	bernoulli_distribution left_right_dist(0.5);
	const bool left = left_right_dist(rnd::engine);
	const double x0 = left ? x_margin : _config.get_screen_w() - x_margin;
	const double x1 = left ? _config.get_screen_w() - x_margin : x_margin;

	vector<point> points { { x0, y0 }, { x0, y1 }, { x1, y1 }, { x1, y2 } };

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(points.front().x, points.front().y, 1.57);
	auto shape = cmp::create_circle(points.front().x, points.front().y, 55.0);
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::SHIPS;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER),
			_resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER_FLASH)); 

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_path_dynamics(points);
	auto movement_bounds = shared_ptr<cmp::bounds>();
	auto life_bounds = cmp::create_bounds(
		-offscreen,
		-offscreen - 1.0, // -1 ensures that the starting position is valid
		_config.get_screen_w() + offscreen,
		_config.get_screen_h() + offscreen - 1.0); // -1 ensures that the
							   // end position is invalid

	// Arms components.
	// ----------------
	auto ammo = cmp::create_ammo_unlimited();
	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(1.5, 1.5, +25.0, -5.0),
			cmp::create_period_bullet(1.5, 1.5, -25.0, -5.0),
			cmp::create_period_missile(4.0, 4.0, +40.0, -5.0),
			cmp::create_period_missile(4.0, 4.0, -40.0, -5.0) });

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue();
	auto cc = cmp::coll_class::ENEMY_SHIP;

	// Pain components.
	// ----------------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::PLAYER_BULLET, 5.0 },
		{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
		{ cmp::coll_class::PLAYER_SHIP, 30.0 },
		{ cmp::coll_class::DEBRIS, 10.0 }
	});

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(50.0); 
	shared_ptr<cmp::timer> ttl;
	vector<shared_ptr<cmp::reaction>> reactions {
		cmp::create_debris_reaction(5, {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 },
			100.0, 300.0,
			4.0, 7.0,
			/* explode = */ false,
			/* randomize = */ true),
		cmp::create_explosion_sequence_reaction(3) };

	bernoulli_distribution drop_health(0.125);
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.1);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);

	// Fx components.
	// --------------
	auto fxs = cmp::create_smoke_when_hurt(0.25);

	// Score components.
	// -----------------
	auto sc = cmp::score_class::ENEMY_HEAVY_FIGHTER;

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_light_bomber() {

	// Helpers.
	// --------
	bernoulli_distribution xdir_dist;

	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	
	const double vx = dir * 50.0;
	const double vy = 50.0;

	const double offscreen = 30.0;
	const double x = (dir > 0.0) ? -offscreen : _config.get_screen_w() + offscreen;
	const double y = -offscreen;

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 1.57);
	auto pain_flash = make_shared<double>(0.0);
	auto shape = cmp::create_complex_shape({
		cmp::create_circle(x - 25.0, y - 36.0, 36.0),
		cmp::create_circle(x + 25.0, y - 36.0, 36.0),
		cmp::create_circle(x, y + 36.0, 36.0)
	});

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::SHIPS;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER),
			_resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER_FLASH)); 

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
	auto movement_bounds = shared_ptr<cmp::bounds>();
	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	// Arms components.
	// ----------------
	auto ammo = cmp::create_ammo_unlimited();
	auto weapon_beh = cmp::create_complex_weapon_beh({
		cmp::create_period_bullet(1.0, 1.0, +55.0, -30.0),
		cmp::create_period_bullet(1.0, 1.0, -55.0, -30.0),
		cmp::create_period_missile(3.0, 3.0, +65.0, -30.0),
		cmp::create_period_missile(3.0, 3.0, -65.0, -30.0)
	});

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue();
	auto cc = cmp::coll_class::ENEMY_SHIP;

	// Pain components.
	// ----------------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::PLAYER_BULLET, 5.0 },
		{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
		{ cmp::coll_class::PLAYER_SHIP, 30.0 },
		{ cmp::coll_class::DEBRIS, 10.0 }
	});

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(70.0); 
	vector<shared_ptr<cmp::reaction>> reactions {
		cmp::create_debris_reaction(7, {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 },
			100.0, 300.0,
			4.0, 7.0,
			/* explode = */ false,
			/* randomize = */ true),
		cmp::create_explosion_sequence_reaction(5)
	};

	bernoulli_distribution drop_health(0.333);
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.125);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);
	shared_ptr<cmp::timer> ttl;

	// Fx components.
	// --------------
	auto fxs = cmp::create_smoke_when_hurt(0.25);

	// Score components.
	// -----------------
	auto sc = cmp::score_class::ENEMY_LIGHT_BOMBER;

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_heavy_bomber() {

	// Helpers.
	// --------
	bernoulli_distribution xdir_dist;
	uniform_real_distribution<double> y_dist(
			50.0, _config.get_screen_h() * 0.5);

	const double vx = 40.0;
	const double vy = 10.0;

	const double offscreen = 100.0;
	const double x = -offscreen;
	const double y = y_dist(rnd::engine);

	// Common components.
	// -----------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0);
	auto pain_flash = make_shared<double>(0.0);
	auto shape = cmp::create_complex_shape({
		cmp::create_circle(x - 100.0, y + 15, 50.0),
		cmp::create_circle(x, y + 15, 50.0),
		cmp::create_circle(x + 100.0, y + 15, 50.0)
	});

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::SHIPS;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER),
			_resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER_FLASH)); 

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
	auto movement_bounds = shared_ptr<cmp::bounds>();
	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	// Arms components.
	// ----------------
	auto ammo = cmp::create_ammo_unlimited();
	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(1.0, 1.0, +55.0, -30.0),
			cmp::create_period_bullet(1.0, 1.0, -55.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, +65.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, -65.0, -30.0) });

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue();
	auto cc = cmp::coll_class::ENEMY_SHIP;

	// Pain components.
	// ----------------
	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::PLAYER_BULLET, 5.0 },
		{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
		{ cmp::coll_class::PLAYER_SHIP, 30.0 },
		{ cmp::coll_class::DEBRIS, 10.0 }
	});

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(90.0); 
	vector<shared_ptr<cmp::reaction>> reactions {
		cmp::create_debris_reaction(13, {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 },
			100.0, 300.0,
			4.0, 7.0,
			/* explode = */ false,
			/* randomize = */ true),
		cmp::create_explosion_sequence_reaction(7)
	};

	bernoulli_distribution drop_health(0.5);
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.333);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);
	shared_ptr<cmp::timer> ttl;

	// Fx Components.
	// --------------
	auto fxs = cmp::create_smoke_when_hurt(0.25);

	// Score components.
	// -----------------
	auto sc = cmp::score_class::ENEMY_HEAVY_BOMBER;

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_health_pickup(double x, double y, double vx, double vy) {

	// Helpers.
	// --------
	bernoulli_distribution dir_dist;

	uniform_real_distribution<double> mv_dist(15.0, 25.0);
	double base_vx = mv_dist(rnd::engine);
	double base_vy = mv_dist(rnd::engine);
	double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
	double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

	uniform_real_distribution<double> rot_dist(3.0, 9.0);
	double base_av = rot_dist(rnd::engine);
	double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

	// Components.
	// -----------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	auto shape = cmp::create_circle(x, y, 16.0); 
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::FX;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::HEALTH),
			_resman.get_bitmap(res_id::HEALTH));

	// Movement components.
	// --------------------
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 
	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(
			vx + base_vx * mul_vx,
			vy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
			base_av * mul_av) });

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue(); 
	auto cc = cmp::coll_class::HEALTH_PICKUP;

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });

	return id;
}

uint64_t entity_factory::create_missiles_pickup(double x, double y, double vx, double vy) {

	// Helpers.
	// --------
	bernoulli_distribution dir_dist;

	uniform_real_distribution<double> mv_dist(15.0, 25.0);
	double base_vx = mv_dist(rnd::engine);
	double base_vy = mv_dist(rnd::engine);
	double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
	double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

	uniform_real_distribution<double> rot_dist(3.0, 9.0);
	double base_av = rot_dist(rnd::engine);
	double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

	// Initialize components.
	// ----------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	auto shape = cmp::create_circle(x, y, 16.0); 
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::FX;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::MISSILES),
			_resman.get_bitmap(res_id::MISSILES));

	// Movement components.
	// --------------------
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 
	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(
			vx + base_vx * mul_vx,
			vy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
			base_av * mul_av) });

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue(); 
	auto cc = cmp::coll_class::MISSILES_PICKUP;

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });

	return id;
}

uint64_t entity_factory::create_missile(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy,
		uint64_t origin_id) {

	// Helpers.
	// ----------
	const double missile_health = 1.0;
	const double ax = 0;
	const double ay = (vy > 0) ? 500.0 : -500.0;

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, theta); 
	auto shape = cmp::create_circle(x, y, 8.0); 
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::PROJECTILES;
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::MISSILE),
			_resman.get_bitmap(res_id::MISSILE));

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_acc_dynamics(vx, vy, ax, ay);
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue(); 
	auto cc = enemy ? cmp::coll_class::ENEMY_MISSILE : cmp::coll_class::PLAYER_MISSILE;

	// Pain components.
	// ----------------
	auto painmap = enemy
		? cmp::create_painmap({ { cmp::coll_class::PLAYER_SHIP, missile_health } })
		: cmp::create_painmap({ { cmp::coll_class::ENEMY_SHIP, missile_health } });

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(missile_health); 
	auto on_death = cmp::create_complex_reaction({
			cmp::create_debris_reaction(3, {
				res_id::DEBRIS1,
				res_id::DEBRIS2,
				res_id::DEBRIS3,
				res_id::DEBRIS4,
				res_id::DEBRIS5 },
				100.0, 300.0,
				4.0, 7.0,
				/* explode = */ false,
				/* randomize = */ true),
			cmp::create_explosion_sequence_reaction(1) });

	shared_ptr<cmp::timer> ttl;

	// Fx components.
	// --------------
	auto fxs = cmp::create_period_smoke(0.1, 0.125);

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds}); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash }); 
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, shape, wellness, fxs });

	return id;
}

uint64_t entity_factory::create_bullet(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy,
		uint64_t origin_id) {

	// Helpers.
	// --------
	double bullet_health = 1.0;

	// Common components.
	// ------------------
	uint64_t id = ++_last_id;
	auto orientation = cmp::create_orientation(x, y, theta); 
	auto shape = cmp::create_circle(x, y, 8.0); 
	auto pain_flash = make_shared<double>(0.0);

	// Drawing components.
	// -------------------
	auto draw_plane = cmp::draw_plane::PROJECTILES;
	auto appearance = enemy
		? cmp::create_static_bmp(
			_resman.get_bitmap(res_id::EYE_BULLET),
			_resman.get_bitmap(res_id::EYE_BULLET))
		: cmp::create_static_bmp(
			_resman.get_bitmap(res_id::PLAYER_BULLET),
			_resman.get_bitmap(res_id::PLAYER_BULLET));

	// Movement components.
	// --------------------
	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
	auto movement_bounds = shared_ptr<cmp::bounds>();
	auto life_bounds = cmp::create_bounds(0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

	// Collision components.
	// ---------------------
	auto coll_queue = cmp::create_coll_queue(); 
	auto cc = enemy ? cmp::coll_class::ENEMY_BULLET : cmp::coll_class::PLAYER_BULLET;

	// Pain components.
	// ----------------
	auto painmap = enemy
		? cmp::create_painmap({ { cmp::coll_class::PLAYER_SHIP, bullet_health } })
		: cmp::create_painmap({ { cmp::coll_class::ENEMY_SHIP, bullet_health } });

	// Wellness components.
	// --------------------
	auto wellness = cmp::create_wellness(bullet_health); 
	shared_ptr<cmp::timer> ttl; 
	shared_ptr<cmp::reaction> on_death;

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds }); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue }); 
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash }); 
	_wellness_system.add_node({ id, on_death, orientation, shape, dynamics, wellness, ttl });

	return id;
}

