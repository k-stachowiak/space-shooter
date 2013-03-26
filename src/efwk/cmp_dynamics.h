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

#ifndef CMP_DYNAMICS_H
#define CMP_DYNAMICS_H

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "../geometry/types.h"

namespace cmp {

class dynamics {
protected:
	double _vx;
	double _vy;
	double _theta;
public:
	dynamics() : _vx(0), _vy(0), _theta(0) {}
	virtual ~dynamics() {}
	virtual void update(double dt) = 0;
	virtual void input(map<int, bool>& keys) = 0;

	double get_vx() const { return _vx; }
	double get_vy() const { return _vy; }
	double get_theta() const { return _theta; }
};

shared_ptr<dynamics> create_complex_dynamics(vector<shared_ptr<dynamics>> ds);
shared_ptr<dynamics> create_const_velocity_dynamics(double vx, double vy);
shared_ptr<dynamics> create_const_acc_dynamics(double vx0, double vy0, double ax, double ay);
shared_ptr<dynamics> create_const_ang_vel_dynamics(double theta);
shared_ptr<dynamics> create_player_controlled_dynamics();
shared_ptr<dynamics> create_path_dynamics(vector<point> points, double lin_vel);

}

#endif /* CMP_DYNAMICS_H_ */
