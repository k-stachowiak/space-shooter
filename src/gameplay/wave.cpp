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

static void zorro_loc_dyn(
		double screen_w,
		double screen_h,
		double& x,
		double& y,
		shared_ptr<cmp::dynamics>& dyn) {

	// Create path.
	// ------------
	uniform_real_distribution<double> x_margin_dist(50.0, 200.0);

	const double x_margin = x_margin_dist(rnd::engine);

	const double y0 = -narrow_offscreen;
	const double y1 = screen_h * 0.5;
	const double y2 = screen_h + narrow_offscreen;

	bernoulli_distribution left_right_dist(0.5);
	const bool left = left_right_dist(rnd::engine);
	const double x0 = left ? x_margin : screen_w - x_margin;
	const double x1 = left ? screen_w - x_margin : x_margin;

	vector<point> points { { x0, y0 }, { x0, y1 }, { x1, y1 }, { x1, y2 } };

	// Set the result.
	// ---------------
	dyn = cmp::create_path_dynamics(points);
	x = points.front().x;
	y = points.front().y;
}

static void diag_loc_dyn(
		double screen_w,
		double screen_h,
		double& x,
		double& y,
		shared_ptr<cmp::dynamics>& dyn) {

	// Determine location and velocities.
	// ----------------------------------
	bernoulli_distribution xdir_dist;
	const double dir = xdir_dist(rnd::engine) ? 1.0 : -1.0;
	const double vx = dir * 175.0;
	const double vy = 175.0;

	// Set the result.
	// ---------------
	dyn = cmp::create_const_velocity_dynamics(vx, vy);
	x = (dir > 0.0) ? -narrow_offscreen : screen_w + narrow_offscreen;
	y = -narrow_offscreen;
}

static void vert_loc_dyn(
		double screen_w,
		double screen_h,
		double& x,
		double& y,
		shared_ptr<cmp::dynamics>& dyn) {

	// Select random parameters.
	// -------------------------
	uniform_real_distribution<double> x_dist(
			50.0, screen_w - 50.0);

	const double vx = 0.0;
	const double vy = 130.0;

	// Set the result.
	// ---------------
	dyn = cmp::create_const_velocity_dynamics(vx, vy);
	x = x_dist(rnd::engine);
	y = -narrow_offscreen;
}

static void hori_loc_dyn(
		double screen_w,
		double screen_h,
		double& x,
		double& y,
		shared_ptr<cmp::dynamics>& dyn) {

	// Select random parameters.
	// -------------------------
	uniform_real_distribution<double> y_dist(
			50.0, screen_w - 50.0);

	const double vx = 600.0;
	const double vy = 0.0;

	// Set the result.
	// ---------------
	dyn = cmp::create_const_velocity_dynamics(vx, vy);
	x = -narrow_offscreen;
	y = y_dist(rnd::engine);
}

static void spawn_pattern(
		pattern& pat,
		entity_factory& ef,
		double screen_w,
		double screen_h) {

	// Determine the location and dynamics.
	// ------------------------------------
	double x, y;
	shared_ptr<cmp::dynamics> dynamics;
	switch(pat.movement) {
	case movement_type::zorro:
		zorro_loc_dyn(screen_w, screen_h, x, y, dynamics);
		break;

	case movement_type::diagonal:
		diag_loc_dyn(screen_w, screen_h, x, y, dynamics);
		break;

	case movement_type::vertical:
		vert_loc_dyn(screen_w, screen_h, x, y, dynamics);
		break;

	case movement_type::horizontal:
		hori_loc_dyn(screen_w, screen_h, x, y, dynamics);
		break;

	default:
		break;
	}

	// Create an enemy for each pattern element.
	// -----------------------------------------
	for(pattern::element const& e : pat.elements) {

		const double ex = x + e.x_off;
		const double ey = y + e.y_off;

		switch(e.type) {
		case enemy_type::light_fighter:
			ef.create_light_fighter_dyn(ex, ey, dynamics);
			break;

		case enemy_type::heavy_fighter:
			ef.create_heavy_fighter_dyn(ex, ey, dynamics);
			break;

		case enemy_type::light_bomber:
			ef.create_light_bomber_dyn(ex, ey, dynamics);
			break;

		case enemy_type::heavy_bomber:
			ef.create_heavy_bomber_dyn(ex, ey, dynamics);
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
