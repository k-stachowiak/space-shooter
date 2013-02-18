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

#include <tuple>
using std::tie;

#include "../misc/rand.h"

#include "cmp_timer.h"
#include "cmp_shape.h"
#include "cmp_fx.h"

namespace cmp {


class smoke_when_hurt : public fx {
	shared_ptr<timer> _timer;
	double _pain_threshold;
public:
	smoke_when_hurt(shared_ptr<timer> t, double pain_threshold)
	: _timer(t)
	, _pain_threshold(pain_threshold)
	{}

	void update(
			double dt,
			double health_ratio,
			double x, double y,
			shared_ptr<cmp::shape> shape,
			comm::msg_queue& msgs) {

		if(health_ratio > _pain_threshold)
			return;

		_timer->update(dt);
		for(uint32_t i = 0; i < _timer->get_ticks(); ++i) {
			double rnd_x, rnd_y;
			tie(rnd_x, rnd_y) = shape->get_random_point();
			msgs.push(comm::create_spawn_smoke(rnd_x, rnd_y, comm::smoke_size::medium));
		}
		_timer->clear();
	}
};

class period_smoke : public fx {
	double _dt_min;
	double _dt_max;
	double _counter;

	void init_counter(double remainder = 0.0) {
		uniform_real_distribution<double> distr(_dt_min, _dt_max);
		_counter = distr(rnd::engine) - remainder;
	}

public:
	period_smoke(double dt_min, double dt_max)
	: _dt_min(dt_min)
	, _dt_max(dt_max) {
		init_counter();
	}

	void update(	double dt,
			double health_ratio,
			double x, double y,
			shared_ptr<cmp::shape> shape,
			comm::msg_queue& msgs) {
		_counter -= dt;
		if(_counter <= 0.0) {
			init_counter(-_counter);
			msgs.push(comm::create_spawn_smoke(x, y, comm::smoke_size::tiny));
		}
	}
};

shared_ptr<fx> create_smoke_when_hurt(shared_ptr<timer> t, double pain_threshold) {
	return shared_ptr<fx>(new smoke_when_hurt(t, pain_threshold));
}

shared_ptr<fx> create_period_smoke(double dt_min, double dt_max) {
	return shared_ptr<fx>(new period_smoke(dt_min, dt_max));
}

}
