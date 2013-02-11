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

#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "comm.h"
#include "systems.h"
#include "../config.h"
#include "../resources/resman.h"

class entity_factory {

	uint64_t _last_id;

	config const& _config;
	resman const& _resman;

	sys::movement_system&	_movement_system;
	sys::collision_system&	_collision_system;
	sys::arms_system&	_arms_system;
	sys::pain_system&	_pain_system;
	sys::wellness_system&	_wellness_system;
	sys::fx_system&		_fx_system;
	sys::drawing_system&	_drawing_system;
	sys::score_system&	_score_system;
	sys::pickup_system&	_pickup_system;

public:
	entity_factory(
		config const& cfg,
		resman const& rm,
		sys::movement_system&	movement_system,
		sys::collision_system&	collision_system,
		sys::arms_system&	arms_system,
		sys::pain_system&	pain_system,
		sys::wellness_system&	wellness_system,
		sys::fx_system&		fx_system,
		sys::drawing_system&	drawing_system,
		sys::score_system&	score_system,
		sys::pickup_system&	pickup_system)
	: _last_id(0)
	, _config(cfg)
	, _resman(rm)
	, _movement_system (movement_system)
	, _collision_system (collision_system)
	, _arms_system (arms_system)
	, _pain_system (pain_system)
	, _wellness_system (wellness_system)
	, _fx_system (fx_system)
	, _drawing_system (drawing_system)
	, _score_system (score_system) 
	, _pickup_system (pickup_system)
	{}

	uint64_t create_explosion(double x, double y);
	uint64_t create_smoke(double x, double y, comm::smoke_size size);

	uint64_t create_debris(double x, double y,
			double bvx, double bvy,
			double vmin, double vmax,
			double theta_min, double theta_max,
			res_id bmp,
			bool explode);

	uint64_t create_star();
	uint64_t create_player_ship(double x, double y);

	uint64_t create_bomber();
	uint64_t create_eye();

	uint64_t create_light_fighter();
	uint64_t create_heavy_fighter();
	uint64_t create_light_bomber();
	uint64_t create_heavy_bomber();

	uint64_t create_health_pickup(double x, double y, double vx, double vy);
	uint64_t create_missiles_pickup(double x, double y, double vx, double vy);

	uint64_t create_missile(
			double x, double y,
			double theta,
			double vx, double vy,
			bool enemy,
			uint64_t origin_id);

	uint64_t create_bullet(
			double x, double y,
			double theta,
			double vx, double vy,
			bool enemy,
			uint64_t origin_id);
};

#endif
