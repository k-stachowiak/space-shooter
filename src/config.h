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

#ifndef CONFIG_H
#define CONFIG_H

class config {
public:
	double get_fps() const {
		return 50.0;
	}

	double get_screen_w() const {
		return 800.0;
	}

	double get_screen_h() const {
		return 800.0;
	}

	double get_player_vx() const {
		return 400.0f;
	}

	double get_player_vy() const {
		return 300.0f;
	}

	double get_player_max_health() const {
		return 100.0;
	}

	double get_player_max_armor() const {
		return 100.0;
	}

	double get_shoot_period() const {
		return 0.1;
	}

	double get_player_bullet_v() const {
		return 1000.0;
	}

	double get_eye_bullet_v() const {
		return 500.0;
	}

	double get_near_star_v() const {
		return 100.0;
	}

	double get_eye_v() const {
		return 200.0f;
	}
};

#endif
