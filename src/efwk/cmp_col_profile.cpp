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

#include "cmp_misc.h"
#include "cmp_col_profile.h"

namespace cmp {

class simple_damage_profile : public damage_profile {
	double _amount;
public:
	simple_damage_profile(double amount) : _amount(amount) {}
	double compute_pain(pain_profile) {
		return _amount;
	}
};

class health_pickup_profile : public pickup_profile {
	double _amount;
public:
	health_pickup_profile(double amount) : _amount(amount) {}
	bool trigger(shared_ptr<wellness> w,
			shared_ptr<ammo> a,
			shared_ptr<upgrades> up) {

		double h = w->get_health();
		double H = w->get_max_health();

		if(h >= H) return false;

		if(h + _amount >= H) w->add_health(H - h);
		else w->add_health(_amount);

		return true;
	}
};

class missiles_pickup_profile : public pickup_profile {
	double _amount;
public:
	missiles_pickup_profile(double amount) : _amount(amount) {}
	bool trigger(shared_ptr<wellness> w,
			shared_ptr<ammo> a,
			shared_ptr<upgrades> up) {
		a->add_rockets(_amount);
		return true;
	}
};

class bullet_upgrade_pickup_profile : public pickup_profile {
public:
	bool trigger(shared_ptr<wellness> w,
				 shared_ptr<ammo> a,
				 shared_ptr<upgrades> up) {
		up->upgrade_gun();
		return true;
	}
};

shared_ptr<collision_profile> create_collision_profile(
		pain_team pain_t,
		pain_profile pain_p,
		bool is_projectile,
		unique_ptr<damage_profile> dmg_p,
		unique_ptr<pickup_profile> pick_p) {

	return shared_ptr<collision_profile>(new collision_profile {
		pain_t,
		pain_p,
		is_projectile,
		move(dmg_p),
		move(pick_p)
	});
}

unique_ptr<damage_profile> create_simple_damage_profile(double amount) {
	return unique_ptr<damage_profile>(new simple_damage_profile(amount));
}

unique_ptr<pickup_profile> create_health_pickup_profile(double amount) {
	return unique_ptr<pickup_profile>(new health_pickup_profile(amount));
}

unique_ptr<pickup_profile> create_missiles_pickup_profile(double amount) {
	return unique_ptr<pickup_profile>(new missiles_pickup_profile(amount));
}

unique_ptr<pickup_profile> create_bullet_upgrade_pickup_profile() {
	return unique_ptr<pickup_profile>(new bullet_upgrade_pickup_profile());
}

}