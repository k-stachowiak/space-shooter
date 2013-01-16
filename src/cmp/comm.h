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

namespace comm {

enum class smoke_size {
	small, medium, big
};

enum class msg_t {
	remove_entity,
	spawn_bullet,
	spawn_missile,
	spawn_explosion,
	spawn_smoke,
	spawn_debris
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
		bool enemy;
		uint64_t origin_id;
	} spawn_bullet;

	struct {
		double x, y;
		double theta;
		double vx, vy;
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
	} spawn_debris;
};

message create_spawn_bullet(double x, double y, double theta, double vx, double vy, bool enemy, uint64_t origin_id);
message create_spawn_missile(double x, double y, double theta, double vx, double vy, bool enemy, uint64_t origin_id);
message create_spawn_explosion(double x, double y);
message create_spawn_smoke(double x, double y, smoke_size size);
message create_spawn_debris(double x, double y, double vx, double vy);
message create_remove_entity(uint64_t id);

}

#endif
