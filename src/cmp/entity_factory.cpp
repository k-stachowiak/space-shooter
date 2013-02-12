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

	// Initialize components.
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::FX;
	
	uint32_t frame_width = 70;
	uint32_t num_frames = 16;
	double frame_time = 0.05;
	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(res_id::EXPLOSION),
			_resman.get_bitmap(res_id::EXPLOSION),
			frame_width,
			num_frames,
			frame_defs,
			1);

	shared_ptr<cmp::dynamics> dynamics; 
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	shared_ptr<cmp::shape> shape; 
	shared_ptr<cmp::wellness> wellness; 
	auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 

	shared_ptr<cmp::reaction> on_death;

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_smoke(double x, double y, comm::smoke_size size) {

	// Initialize components.
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::FX;
	
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
	
	uint32_t frame_width = 32 * 3 * scale;
	uint32_t num_frames = 25;
	double frame_time = 0.075;
	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(rid),
			_resman.get_bitmap(rid),
			frame_width,
			num_frames,
			frame_defs,
			1);

	shared_ptr<cmp::dynamics> dynamics; 
	auto orientation = cmp::create_orientation(x, y, 0.0); 
	shared_ptr<cmp::shape> shape; 
	shared_ptr<cmp::wellness> wellness; 
	auto ttl = cmp::create_const_int_timer(num_frames * frame_time); 

	shared_ptr<cmp::reaction> on_death;

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_debris(double x, double y,
		double bvx, double bvy,
		double vmin, double vmax,
		double theta_min, double theta_max,
		res_id bmp,
		bool explode) {

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

	// Initialize components.
	uint64_t id = ++_last_id;
	
	cmp::draw_plane draw_plane = cmp::draw_plane::FX;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(bmp),
			_resman.get_bitmap(bmp));

	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(
				bvx + base_vx * mul_vx,
				bvy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
				base_av * mul_av) });

	auto orientation = cmp::create_orientation(x, y, 0.0); 
	shared_ptr<cmp::shape> shape; 
	shared_ptr<cmp::wellness> wellness; 
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 
	auto ttl = cmp::create_const_int_timer(ttl_time); 

	shared_ptr<cmp::reaction> on_death;
	if(explode) on_death = cmp::create_explosion_sequence_reaction(1);

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_star() {

	// Helpers
	uniform_real_distribution<double> uni_dist;
	double grade = uni_dist(rnd::engine);

	double vy = 200.0 * (grade * 0.5 + 0.5);
	double shade = grade * 0.9 + 0.1;
	
	double x = uni_dist(rnd::engine) * _config.get_screen_w();
	
	// Initialize components.
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::BACKGROUND;

	auto appearance = cmp::create_pixel(shade, shade, shade);

	auto orientation = cmp::create_orientation(x, 1.0, 0.0);

	auto pain_flash = make_shared<double>(0.0);

	shared_ptr<cmp::shape> shape;

	auto dynamics = cmp::create_const_velocity_dynamics(0.0, vy);

	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });

	return id;
}

uint64_t entity_factory::create_player_ship(double x, double y) {

	// Initialize components.
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::SHIPS;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::PLAYER_SHIP),
			_resman.get_bitmap(res_id::PLAYER_SHIP_FLASH));

	shared_ptr<cmp::dynamics> dynamics; 
	auto orientation = cmp::create_orientation(x, y, -1.57); 

	auto movement_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 

	auto life_bounds = shared_ptr<cmp::bounds>(); 
	auto cc = cmp::coll_class::PLAYER_SHIP; 
	auto shape = cmp::create_circle(x, y, 32.0); 
	auto coll_queue = cmp::create_coll_queue(); 
	shared_ptr<cmp::weapon_beh> weapon_beh; 

	auto painmap = cmp::create_painmap({
		{ cmp::coll_class::ENEMY_BULLET, 10.0 },
		{ cmp::coll_class::ENEMY_MISSILE, 20.0 },
		{ cmp::coll_class::ENEMY_SHIP, 25.0 }
	}); 

	auto wellness = cmp::create_wellness(100.0); 

	auto ammo = cmp::create_ammo(-1, 3);

	shared_ptr<cmp::timer> ttl; 
	auto fxs = cmp::create_smoke_when_hurt(0.25);

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
			cmp::create_explosion_sequence_reaction(7) });

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });

	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});

	_arms_system.add_node({id, orientation, weapon_beh, ammo });

	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_pickup_system.add_node({ id, coll_queue, wellness, ammo });

	return id;
}

uint64_t entity_factory::create_light_fighter() {

	bernoulli_distribution xdir_dist;

	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	
	const double vx = dir * 75.0;
	const double vy = 75.0;

	const double offscreen = 30.0;
	const double x = (dir > 0.0)
		? -offscreen
		: _config.get_screen_w() + offscreen;
	const double y = -offscreen;

	// Prepare the components.
	// -----------------------
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::SHIPS;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER),
			_resman.get_bitmap(res_id::ENEMY_LIGHT_FIGHTER_FLASH)); 

	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);

	auto orientation = cmp::create_orientation(x, y, 1.57);

	auto movement_bounds = shared_ptr<cmp::bounds>();

	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	auto cc = cmp::coll_class::ENEMY_SHIP;

	auto shape = cmp::create_circle(x, y, 32.0);

	auto coll_queue = cmp::create_coll_queue();

	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(3.0, 3.0, +17.0, -3.0),
			cmp::create_period_bullet(3.0, 3.0, -17.0, -3.0) });

	auto painmap = cmp::create_painmap({
			{ cmp::coll_class::PLAYER_BULLET, 10.0 },
			{ cmp::coll_class::PLAYER_MISSILE, 30.0 },
			{ cmp::coll_class::PLAYER_SHIP, 50.0 } });

	auto wellness = cmp::create_wellness(30.0); 

	auto ammo = cmp::create_ammo_unlimited();

	shared_ptr<cmp::timer> ttl;

	auto fxs = cmp::create_smoke_when_hurt(0.25);

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
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.05);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);

	auto sc = cmp::score_class::ENEMY_LIGHT_FIGHTER;

	auto pain_flash = make_shared<double>(0.0);

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_heavy_fighter() {

	// Prepare helpers.
	// ----------------

	// Determine path points.
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

	// Prepare the components.
	// -----------------------
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::SHIPS;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER),
			_resman.get_bitmap(res_id::ENEMY_HEAVY_FIGHTER_FLASH)); 

	auto dynamics = cmp::create_path_dynamics(points);

	auto orientation = cmp::create_orientation(points.front().x, points.front().y, 1.57);

	auto movement_bounds = shared_ptr<cmp::bounds>();

	auto life_bounds = cmp::create_bounds(
		-offscreen,
		-offscreen - 1.0, // -1 ensures that the starting position is valid
		_config.get_screen_w() + offscreen,
		_config.get_screen_h() + offscreen - 1.0); // -1 ensures that the
							   // end position is invalid

	auto cc = cmp::coll_class::ENEMY_SHIP;

	auto shape = cmp::create_circle(points.front().x, points.front().y, 55.0);

	auto coll_queue = cmp::create_coll_queue();

	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(1.5, 1.5, +25.0, -5.0),
			cmp::create_period_bullet(1.5, 1.5, -25.0, -5.0),
			cmp::create_period_missile(4.0, 4.0, +40.0, -5.0),
			cmp::create_period_missile(4.0, 4.0, -40.0, -5.0) });

	auto painmap = cmp::create_painmap({
			{ cmp::coll_class::PLAYER_BULLET, 5.0 },
			{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
			{ cmp::coll_class::PLAYER_SHIP, 30.0 } });

	auto wellness = cmp::create_wellness(50.0); 

	auto ammo = cmp::create_ammo_unlimited();

	shared_ptr<cmp::timer> ttl;

	auto fxs = cmp::create_smoke_when_hurt(0.25);

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

	auto sc = cmp::score_class::ENEMY_HEAVY_FIGHTER;

	auto pain_flash = make_shared<double>(0.0);

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_light_bomber() {

	// Prepare helpers.
	// ----------------
	bernoulli_distribution xdir_dist;

	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	
	const double vx = dir * 50.0;
	const double vy = 50.0;

	const double offscreen = 30.0;
	const double x = (dir > 0.0)
		? -offscreen
		: _config.get_screen_w() + offscreen;
	const double y = -offscreen;

	// Prepare the components.
	// -----------------------
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::SHIPS;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER),
			_resman.get_bitmap(res_id::ENEMY_LIGHT_BOMBER_FLASH)); 

	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);

	auto orientation = cmp::create_orientation(x, y, 1.57);

	auto movement_bounds = shared_ptr<cmp::bounds>();

	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	auto cc = cmp::coll_class::ENEMY_SHIP;

	auto shape = cmp::create_complex_shape({
		cmp::create_circle(x - 25.0, y - 36.0, 36.0),
		cmp::create_circle(x + 25.0, y - 36.0, 36.0),
		cmp::create_circle(x, y + 36.0, 36.0)
	});

	auto coll_queue = cmp::create_coll_queue();

	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(1.0, 1.0, +55.0, -30.0),
			cmp::create_period_bullet(1.0, 1.0, -55.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, +65.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, -65.0, -30.0) });

	auto painmap = cmp::create_painmap({
			{ cmp::coll_class::PLAYER_BULLET, 5.0 },
			{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
			{ cmp::coll_class::PLAYER_SHIP, 30.0 } });

	auto wellness = cmp::create_wellness(70.0); 

	auto ammo = cmp::create_ammo_unlimited();

	shared_ptr<cmp::timer> ttl;

	auto fxs = cmp::create_smoke_when_hurt(0.25);

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
		cmp::create_explosion_sequence_reaction(5) };

	bernoulli_distribution drop_health(0.333);
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.125);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);

	auto sc = cmp::score_class::ENEMY_LIGHT_BOMBER;

	auto pain_flash = make_shared<double>(0.0);

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_heavy_bomber() {

	// Prepare helpers.
	// ----------------
	bernoulli_distribution xdir_dist;
	uniform_real_distribution<double> y_dist(
			50.0, _config.get_screen_h() * 0.5);

	const double vx = 40.0;
	const double vy = 10.0;

	const double offscreen = 100.0;
	const double x = -offscreen;
	const double y = y_dist(rnd::engine);

	// Prepare the components.
	// -----------------------
	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::SHIPS;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER),
			_resman.get_bitmap(res_id::ENEMY_HEAVY_BOMBER_FLASH)); 

	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);

	auto orientation = cmp::create_orientation(x, y, 0.0);

	auto movement_bounds = shared_ptr<cmp::bounds>();

	auto life_bounds = cmp::create_bounds(
		-offscreen - 1.0,
		-offscreen - 1.0,
		_config.get_screen_w() + offscreen + 1.0,
		_config.get_screen_h() + offscreen + 1.0);

	auto cc = cmp::coll_class::ENEMY_SHIP;

	auto shape = cmp::create_complex_shape({
		cmp::create_circle(x - 100.0, y + 15, 50.0),
		cmp::create_circle(x, y + 15, 50.0),
		cmp::create_circle(x + 100.0, y + 15, 50.0)
	});

	auto coll_queue = cmp::create_coll_queue();

	auto weapon_beh = cmp::create_complex_weapon_beh({
			cmp::create_period_bullet(1.0, 1.0, +55.0, -30.0),
			cmp::create_period_bullet(1.0, 1.0, -55.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, +65.0, -30.0),
			cmp::create_period_missile(3.0, 3.0, -65.0, -30.0) });

	auto painmap = cmp::create_painmap({
			{ cmp::coll_class::PLAYER_BULLET, 5.0 },
			{ cmp::coll_class::PLAYER_MISSILE, 20.0 },
			{ cmp::coll_class::PLAYER_SHIP, 30.0 } });

	auto wellness = cmp::create_wellness(90.0); 

	auto ammo = cmp::create_ammo_unlimited();

	shared_ptr<cmp::timer> ttl;

	auto fxs = cmp::create_smoke_when_hurt(0.25);

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
		cmp::create_explosion_sequence_reaction(7) };

	bernoulli_distribution drop_health(0.5);
	if(drop_health(rnd::engine)) reactions.push_back(cmp::create_health_drop_reaction());

	bernoulli_distribution drop_missile(0.333);
	if(drop_missile(rnd::engine)) reactions.push_back(cmp::create_missile_drop_reaction());

	auto on_death = cmp::create_complex_reaction(reactions);

	auto sc = cmp::score_class::ENEMY_HEAVY_BOMBER;

	auto pain_flash = make_shared<double>(0.0);

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh, ammo });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_health_pickup(double x, double y, double vx, double vy) {

	// Prepare helpers.
	// ----------------
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

	cmp::draw_plane draw_plane = cmp::draw_plane::FX;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::HEALTH),
			_resman.get_bitmap(res_id::HEALTH));

	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(
			vx + base_vx * mul_vx,
			vy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
			base_av * mul_av) });

	auto orientation = cmp::create_orientation(x, y, 0.0); 

	auto shape = cmp::create_circle(x, y, 16.0); 
	auto coll_queue = cmp::create_coll_queue(); 
	shared_ptr<cmp::wellness> wellness; 
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 

	shared_ptr<cmp::timer> ttl; 

	auto cc = cmp::coll_class::HEALTH_PICKUP;

	shared_ptr<cmp::reaction> on_death;

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_missiles_pickup(double x, double y, double vx, double vy) {

	// Prepare helpers.
	// ----------------
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

	cmp::draw_plane draw_plane = cmp::draw_plane::FX;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::MISSILES),
			_resman.get_bitmap(res_id::MISSILES));

	auto dynamics = cmp::create_complex_dynamics({
		cmp::create_const_velocity_dynamics(
			vx + base_vx * mul_vx,
			vy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
			base_av * mul_av) });

	auto orientation = cmp::create_orientation(x, y, 0.0); 

	auto shape = cmp::create_circle(x, y, 16.0); 
	auto coll_queue = cmp::create_coll_queue(); 
	shared_ptr<cmp::wellness> wellness; 
	auto movement_bounds = shared_ptr<cmp::bounds>(); 
	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h()); 

	shared_ptr<cmp::timer> ttl; 

	auto cc = cmp::coll_class::MISSILES_PICKUP;

	shared_ptr<cmp::reaction> on_death;

	auto pain_flash = make_shared<double>(0.0);

	// Register nodes.
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics });
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });

	// Feedback for the state.
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

	// Initialize Components.
	// ----------------------

	// Common components.

	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::PROJECTILES;
	
	auto appearance = cmp::create_static_bmp(
			_resman.get_bitmap(res_id::MISSILE),
			_resman.get_bitmap(res_id::MISSILE));

	auto dynamics = cmp::create_const_acc_dynamics(vx, vy, ax, ay);

	auto orientation = cmp::create_orientation(x, y, theta); 
	auto movement_bounds = shared_ptr<cmp::bounds>(); 

	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

	auto shape = cmp::create_circle(x, y, 8.0); 
	auto coll_queue = cmp::create_coll_queue(); 
	auto wellness = cmp::create_wellness(missile_health); 
	shared_ptr<cmp::timer> ttl;

	auto fxs = cmp::create_period_smoke(0.1, 0.125);

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

	auto pain_flash = make_shared<double>(0.0);

	// Context dependent.

	shared_ptr<cmp::painmap> painmap;
	cmp::coll_class cc;
	if(enemy) {
		cc = cmp::coll_class::ENEMY_MISSILE;
		painmap = cmp::create_painmap({ { cmp::coll_class::PLAYER_SHIP, missile_health } });
	} else {
		cc = cmp::coll_class::PLAYER_MISSILE;
		painmap = cmp::create_painmap({ { cmp::coll_class::ENEMY_SHIP, missile_health } });
	}

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds}); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue });
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash }); 
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });

	return id;
}

uint64_t entity_factory::create_bullet(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy,
		uint64_t origin_id) {

	// Constants.
	// ----------
	double bullet_health = 1.0;

	// Initialize components.
	// ----------------------

	// Common components.

	uint64_t id = ++_last_id;

	cmp::draw_plane draw_plane = cmp::draw_plane::PROJECTILES;
	
	auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);

	auto orientation = cmp::create_orientation(x, y, theta); 
	auto movement_bounds = shared_ptr<cmp::bounds>();

	auto life_bounds = cmp::create_bounds(
		0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

	auto shape = cmp::create_circle(x, y, 8.0); 
	auto coll_queue = cmp::create_coll_queue(); 
	auto wellness = cmp::create_wellness(bullet_health); 
	shared_ptr<cmp::timer> ttl; 

	shared_ptr<cmp::reaction> on_death;

	auto pain_flash = make_shared<double>(0.0);

	// Context dependent.

	shared_ptr<cmp::appearance> appearance;
	shared_ptr<cmp::painmap> painmap;
	cmp::coll_class cc;
	if(enemy) {
		appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::EYE_BULLET),
				_resman.get_bitmap(res_id::EYE_BULLET));

		painmap = cmp::create_painmap({ { cmp::coll_class::PLAYER_SHIP, bullet_health } });
		cc = cmp::coll_class::ENEMY_BULLET;
	} else {
		appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::PLAYER_BULLET),
				_resman.get_bitmap(res_id::PLAYER_BULLET));

		painmap = cmp::create_painmap({ { cmp::coll_class::ENEMY_SHIP, bullet_health } });
		cc = cmp::coll_class::PLAYER_BULLET;
	}

	// Register nodes.
	// ---------------
	_drawing_system.add_node({ id, draw_plane, appearance, orientation, shape, pain_flash, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds }); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue }); 
	_pain_system.add_node({ id, coll_queue, painmap, wellness, pain_flash }); 
	_wellness_system.add_node({ id, on_death, orientation, dynamics, wellness, ttl });

	return id;
}
