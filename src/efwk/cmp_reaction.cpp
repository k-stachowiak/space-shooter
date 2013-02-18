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

#include <random>
using std::uniform_real_distribution;
using std::uniform_int_distribution;

#include <tuple>
using std::tie;

#include "../misc/rand.h"

#include "cmp_misc.h"
#include "cmp_shape.h"
#include "cmp_reaction.h"

namespace cmp {

class complex_reaction : public reaction {
	vector<shared_ptr<reaction>> _rs;
public:
	complex_reaction(vector<shared_ptr<reaction>> rs) : _rs(rs) {}
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		for(auto r : _rs)
			r->trigger(x, y, phi, shape, vx, vy, origin_id, queue);
	}
};

class health_drop_reaction : public reaction {
public:
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		queue.push(comm::create_spawn_health_pickup(x, y, vx, vy));
	}
};

class missile_drop_reaction : public reaction {
public:
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		queue.push(comm::create_spawn_missiles_pickup(x, y, vx, vy));
	}
};

class bullet_upgrade_drop_reaction : public reaction {
public:
	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {
		queue.push(comm::create_spawn_bullet_upgrade_pickup(x, y, vx, vy));
	}
};

shared_ptr<reaction> create_complex_reaction(vector<shared_ptr<reaction>> rs) {
	return shared_ptr<reaction>(new complex_reaction(rs));
}

shared_ptr<reaction> create_health_drop_reaction() {
	return shared_ptr<reaction>(new health_drop_reaction);
}

shared_ptr<reaction> create_missile_drop_reaction() {
	return shared_ptr<reaction>(new missile_drop_reaction);
}

shared_ptr<reaction> create_bullet_upgrade_drop_reaction() {
	return shared_ptr<reaction>(new bullet_upgrade_drop_reaction);
}

class debris_reaction : public reaction {
	uint32_t _num_debris;
	vector<res_id> _images;
	double _vmin, _vmax;
	double _theta_min, _theta_max;
	bool _explode;
	bool _randomize;

public:
	debris_reaction(uint32_t num_debris,
			vector<res_id> images,
			double vmin, double vmax,
			double theta_min, double theta_max,
			bool explode,
			bool randomize)
	: _num_debris(num_debris)
	, _images(images)
	, _vmin(vmin)
	, _vmax(vmax)
	, _theta_min(theta_min)
	, _theta_max(theta_max)
	, _explode(explode)
	, _randomize(randomize)
	{}

	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {

		uniform_int_distribution<int> bmp_dist(0, _images.size() - 1);
		for(uint32_t i = 0; i < _num_debris; ++i) {
			uint32_t index = _randomize
				? bmp_dist(rnd::engine)
				: (i % _images.size());
			res_id bmp = _images[index];
			double deb_x, deb_y;
			tie(deb_x, deb_y) = shape->get_random_point();
			queue.push(comm::create_spawn_debris(
						deb_x, deb_y,
						vx, vy,
						_vmin, _vmax,
						_theta_min, _theta_max,
						bmp,
						_explode,
						origin_id));
		}
	}
};

shared_ptr<reaction> create_debris_reaction(
		uint32_t num_debris,
		vector<res_id> images,
		double vmin, double vmax,
		double theta_min, double theta_max,
		bool explode, bool randomize) {
	return shared_ptr<reaction>(new debris_reaction(
				num_debris,
				images,
				vmin, vmax,
				theta_min, theta_max,
				explode,
				randomize));
}

class explosion_sequence_reaction : public reaction {
	uint32_t _num_explosions;
public:
	explosion_sequence_reaction(uint32_t num_explosions)
	: _num_explosions(num_explosions)
	{}

	void trigger(double x, double y, double phi,
			shared_ptr<shape> shape,
			double vx, double vy,
			uint64_t origin_id,
			comm::msg_queue& queue) {

		double delay = 0.0;
		uniform_real_distribution<double> delay_dist(0.125, 0.25);
		for(uint32_t i = 0; i < _num_explosions; ++i) {
			double expl_x, expl_y;
			tie(expl_x, expl_y) = shape->get_random_point();
			queue.push(comm::create_spawn_explosion(expl_x, expl_y), delay);
			delay += delay_dist(rnd::engine);
		}
	}
};

shared_ptr<reaction> create_explosion_sequence_reaction(uint32_t num_explosions) {
	return shared_ptr<reaction>(new explosion_sequence_reaction(num_explosions));
}


}
