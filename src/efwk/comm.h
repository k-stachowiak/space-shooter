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

#ifndef COMM_H
#define COMM_H

#include <stdint.h>

#include <functional>
using std::function;

#include <vector>
using std::vector;

#include <utility>
using std::pair;
using std::move;

#include "../resources/resman.h"

namespace comm {

enum class smoke_size {
	tiny,
	medium,
	big
};

enum class msg_t {
	remove_entity,
	spawn_bullet,
	spawn_missile,
	spawn_explosion,
	spawn_smoke,
	spawn_debris,
	spawn_health_pickup,
	spawn_battery_pickup,
	spawn_missiles_pickup,
	spawn_bullet_upgrade_pickup,
	spawn_missile_upgrade_pickup
};

struct message {

	msg_t type;

	struct {
		uint64_t id;
	} remove_entity;

	struct {
		double x, y;
		double theta;
		double vx, vy;
		size_t upgrade_lvl;
		bool enemy;
		uint64_t origin_id;
	} spawn_bullet;

	struct {
		double x, y;
		double theta;
		double vx, vy;
		size_t upgrade_lvl;
		bool enemy;
		uint64_t origin_id;
	} spawn_missile;

	struct {
		double x, y;
	} spawn_explosion;

	struct {
		double x, y;
		smoke_size size;
	} spawn_smoke;

	struct {
		double x, y;
		double vx, vy;
		double vmin, vmax;
		double theta_min, theta_max;
		res_id image;
		bool explode;
		uint64_t origin_id;
	} spawn_debris;

	struct {
		double x, y;
		double vx, vy;
	} spawn_health_pickup;

	struct {
		double x, y;
		double vx, vy;
	} spawn_battery_pickup;

	struct {
		double x, y;
		double vx, vy;
	} spawn_missiles_pickup;

	struct {
		double x, y;
		double vx, vy;
	} spawn_bullet_upgrade_pickup;

	struct {
		double x, y;
		double vx, vy;
	} spawn_missile_upgrade_pickup;
};

message create_spawn_bullet(
		double x, double y, 
		double theta, 
		double vx, double vy, 
		size_t upgrade_lvl,
		bool enemy, 
		uint64_t origin_id);

message create_spawn_missile(
		double x, double y,
		double theta,
		double vx, double vy,
		size_t upgrade_lvl,
		bool enemy,
		uint64_t origin_id);

message create_spawn_explosion(double x, double y);
message create_spawn_smoke(double x, double y, smoke_size size);

message create_spawn_debris(double x, double y,
		double vx, double vy,
		double vmin, double vmax,
		double theta_min, double theta_max,
		res_id image,
		bool explode,
		uint64_t origin_id);

message create_spawn_health_pickup(double x, double y, double vx, double vy);
message create_spawn_battery_pickup(double x, double y, double vx, double vy);
message create_spawn_missiles_pickup(double x, double y, double vx, double vy);
message create_spawn_bullet_upgrade_pickup(double x, double y, double vx, double vy);
message create_spawn_missile_upgrade_pickup(double x, double y, double vx, double vy);

message create_remove_entity(uint64_t id);

// The message queue container.
// ----------------------------

class msg_queue {
	vector<pair<double, message>> _msgs;
public:
	void push(message const& m, double delay) {
		_msgs.push_back(pair<double, message>(delay, m));
	}

	void push(message const& m) { push(m, 0.0); }

	template<class MsgCallback>
	void for_each_msg(double dt, MsgCallback mc) {
		for(std::size_t i = 0; i < _msgs.size();) {
			pair<double, message>& pr = _msgs[i];
			if(pr.first > 0.0) {
				pr.first -= dt;
				++i;
			} else {
				mc(pr.second);
				_msgs[i] = move(_msgs.back());
				_msgs.pop_back();
				// Note: no increment.
			}
		}
	}
};

}

#endif
