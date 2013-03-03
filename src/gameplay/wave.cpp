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

#include "../misc/rand.h"
#include "wave.h"

static const double narrow_offscreen = 40.0;

static vector<spawn_desc> spawn_zorro_pattern(
		pattern& pat,
		double screen_w,
		double screen_h) {

	// Determine base points.
	// ----------------------
	uniform_real_distribution<double> x_margin_dist(50.0, 200.0);

	const double x_margin = x_margin_dist(rnd::engine);

	const double y0 = -narrow_offscreen;
	const double y1 = screen_h * 0.5;
	const double y2 = screen_h + narrow_offscreen;

	bernoulli_distribution left_right_dist(0.5);
	const bool left = left_right_dist(rnd::engine);
	const double x0 = left ? x_margin : screen_w - x_margin;
	const double x1 = left ? screen_w - x_margin : x_margin;

	// Generate the shifted elements.
	// ------------------------------
	vector<spawn_desc> result;
	for(pattern::element const& e : pat.elements) {

		vector<point> points {
			{ x0 + e.x_off, y0 + e.y_off },
			{ x0 + e.x_off, y1 + e.y_off },
			{ x1 + e.x_off, y1 + e.y_off },
			{ x1 + e.x_off, y2 + e.y_off } };

		double x = points.front().x + e.x_off;
		double y = points.front().y + e.y_off;
		auto dynamics = cmp::create_path_dynamics(points);

		result.push_back({ e.type, x, y, dynamics });
	}

	return result;
}

static vector<spawn_desc> spawn_diagonal_pattern(
		pattern& pat,
		double screen_w,
		double screen_h) {

	// Determine location and velocities.
	// ----------------------------------
	bernoulli_distribution xdir_dist;
	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	const double vx = dir * 175.0;
	const double vy = 175.0;

	double base_x = (dir > 0.0) ? -narrow_offscreen : screen_w + narrow_offscreen;
	double base_y = -narrow_offscreen;

	// Generate the shifted elements.
	// ------------------------------
	vector<spawn_desc> result;
	for(pattern::element const& e : pat.elements) {
		double x = base_x + e.x_off;
		double y = base_y + e.y_off;
		auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
		result.push_back({ e.type, x, y, dynamics });
	}

	return result;
}

static vector<spawn_desc> spawn_vertical_pattern(
		pattern& pat,
		double screen_w,
		double screen_h) {

	// Select random parameters.
	// -------------------------
	uniform_real_distribution<double> x_dist(50.0, screen_w - 50.0);
	const double vx = 0.0;
	const double vy = 130.0;

	double base_x = x_dist(rnd::engine);
	double base_y = -narrow_offscreen;

	// Generate the shifted elements.
	// ------------------------------
	vector<spawn_desc> result;
	for(pattern::element const& e : pat.elements) {
		double x = base_x + e.x_off;
		double y = base_y + e.y_off;
		auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
		result.push_back({ e.type, x, y, dynamics });
	}

	return result;
}

static vector<spawn_desc> spawn_horizontal_pattern(
		pattern& pat,
		double screen_w,
		double screen_h) {

	// Select random parameters.
	// -------------------------
	uniform_real_distribution<double> x_dist(50.0, screen_w - 50.0);
	const double vx = 60.0;
	const double vy = 0.0;

	double base_x = x_dist(rnd::engine);
	double base_y = -narrow_offscreen;

	// Generate the shifted elements.
	// ------------------------------
	vector<spawn_desc> result;
	for(pattern::element const& e : pat.elements) {
		double x = base_x + e.x_off;
		double y = base_y + e.y_off;
		auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);
		result.push_back({ e.type, x, y, dynamics });
	}

	return result;
}

static void spawn_pattern(
		pattern& pat,
		entity_factory& ef,
		double screen_w,
		double screen_h) {

	// Create the spawn descriptors based on the movement type.
	// --------------------------------------------------------
	vector<spawn_desc> spawn_descs;
	switch(pat.movement) {
	case movement_type::zorro:
		spawn_descs = spawn_zorro_pattern(pat, screen_w, screen_h);
		break;

	case movement_type::diagonal:
		spawn_descs = spawn_diagonal_pattern(pat,screen_w, screen_h);
		break;

	case movement_type::vertical:
		spawn_descs = spawn_vertical_pattern(pat, screen_w, screen_h);
		break;

	case movement_type::horizontal:
		spawn_descs = spawn_horizontal_pattern(pat, screen_w, screen_h);
		break;

	default:
		break;
	}

	// Spawn all the elements based on the spawn descriptors.
	// ------------------------------------------------------
	for(spawn_desc const& sd : spawn_descs) {
		switch(sd.type) {
		case enemy_type::light_fighter:
			ef.create_light_fighter_dyn(sd.x, sd.y, sd.dynamics);
			break;

		case enemy_type::heavy_fighter:
			ef.create_heavy_fighter_dyn(sd.x, sd.y, sd.dynamics);
			break;

		case enemy_type::light_bomber:
			ef.create_light_bomber_dyn(sd.x, sd.y, sd.dynamics);
			break;

		case enemy_type::heavy_bomber:
			ef.create_heavy_bomber_dyn(sd.x, sd.y, sd.dynamics);
			break;

		default:
			break;
		}
	}
}

bool wave::tick(double dt, entity_factory& ef, double screen_w, double screen_h) {

	if(_current_pattern >= _patterns.size()) {
		return false;
	}

	pair<double, pattern>& current = _patterns[_current_pattern];
	double& delay = current.first;
	pattern& pat = current.second;

	// Advance clock. See if pattern spawn needed.
	_clock += dt;
	if(_clock < delay) {
		return true;
	}

	// Back the clock up and spawn the pattern.
	_clock -= delay;
	spawn_pattern(pat, ef, screen_w, screen_h);

	// Advance pattern. Are we done?
	++_current_pattern;
	return (_current_pattern < _patterns.size());

}
