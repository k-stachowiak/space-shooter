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

#ifndef CMP_MISC_H
#define CMP_MISC_H

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include <algorithm>
using std::for_each;

#include <iostream>
using std::cout;
using std::endl;

#include "../../misc/delq.h"
#include "../comm.h"

namespace cmp {

class collision_profile;
class shape;

// Component helpers.
// ------------------

// The drawing order management.
enum class draw_plane {
        BACKGROUND,
        SHIPS,
        FX,
        PROJECTILES
};

// Score class id.
enum class score_class {
        PLAYER,
        ENEMY_LIGHT_FIGHTER,
        ENEMY_HEAVY_FIGHTER,
        ENEMY_LIGHT_BOMBER,
        ENEMY_HEAVY_BOMBER
};

// The queue of the noises to be played by the sound system.
typedef del_queue<res::res_id> noise_queue;

// Collision report type.
//
// Note: The origin id is the identifier of the node which
// deals damage through this node. E.g. If this node is a
// bullet, the origin_id will be that of the ship, who has
// launched the bullet. This way the entity responsible
// for the damage may be determined.
struct coll_report {
        uint64_t id;
        uint64_t origin_id;
        shared_ptr<cmp::collision_profile> cp;
        shared_ptr<shape> shp;
};

// Weapon's trigger component.
class weapon {
        double _interval;
        bool _trigger;
        double _counter;

public:
        weapon(double interval)
        : _interval(interval)
        , _trigger(false)
        , _counter(0.0)
        {}

        void set_trigger(bool value) { _trigger = value; }

        bool update(double dt) {
                if(_counter > 0.0) _counter -= dt;
                if(!_trigger || _counter > 0.0) return false;
                _counter += _interval;
                return true;
        }
};

// Formal components.
// ------------------

// The position and the rotation.
class orientation {
        double _x, _y, _phi;
        double _prev_x, _prev_y, _prev_phi;
public:
        orientation(double x, double y, double phi)
        : _x(x), _y(y), _phi(phi)
        {}

        double get_x() const { return _x; }
        double get_y() const { return _y; }
        double get_phi() const { return _phi; }

        void shift_x(double dx) {
                _prev_x = _x;
                _x += dx;
        }

        void shift_y(double dy) {
                _prev_y = _y;
                _y += dy;
        }

        void shift(double dx, double dy) {
                shift_x(dx);
                shift_y(dy);
        }

        void rot(double theta) {
                _prev_phi = _phi;
                _phi += theta;
        }
};

// Universal type defining an AABB box.
class bounds {
        double _x_min;
        double _y_min;
        double _x_max;
        double _y_max;
public:
        bounds(double x_min, double y_min, double x_max, double y_max)
        : _x_min(x_min), _y_min(y_min), _x_max(x_max), _y_max(y_max)
        {}

        double get_x_min() const { return _x_min; }
        double get_y_min() const { return _y_min; }
        double get_x_max() const { return _x_max; }
        double get_y_max() const { return _y_max; }
};

// Collision queue.
class coll_queue {
        vector<coll_report> _queue;
public:
        void clear() { _queue.clear(); }
        void push_report(coll_report cr) { _queue.push_back(cr); }
        void for_each_report(function<void(coll_report const&)> f) const {
                for_each(begin(_queue), end(_queue), f);
        }
};

// The object's health armor etc.
class wellness {
        uint64_t _last_dmg_id; // NOTE that this is used to grant points for kill.
        double _max_health;
        double _health;
        double _max_shield;
        double _shield;
public:
        wellness(double health, double shield)
        : _last_dmg_id(0)
        , _max_health(health), _health(health)
        , _max_shield(shield), _shield(shield)
        {}

        void deal_dmg(double dmg, uint64_t source_id) {
                _shield -= dmg;
                if(_shield < 0) {
                        _health += _shield;
                        _shield = 0;
                }
                _last_dmg_id = source_id;
        }

        void add_health(double dh) { _health += dh; }
        double get_max_health() const { return _max_health; }
        double get_health() const { return _health; }
        void add_shield(double ds) { _shield += ds; }
        double get_max_shield() const { return _max_shield; }
        double get_shield() const { return _shield; }
        bool is_alive() const { return _health > 0.0; }
        uint64_t get_last_dmg_id() const { return _last_dmg_id; }
};

class upgrade_atom {
        unsigned _level;
        unsigned _ticks;
        const unsigned _ticks_per_level;
        const unsigned _level_max;
public:
        upgrade_atom(unsigned level, unsigned ticks_per_level, unsigned level_max)
        : _level(level)
        , _ticks(ticks_per_level)
        , _ticks_per_level(ticks_per_level)
        , _level_max(level_max)
        {}

        bool can_level_up() const {
                return _level < _level_max || _ticks < _ticks_per_level;
        }

        void level_up() {
                if(can_level_up()) {
                        ++_level;
                        _ticks = _ticks_per_level;

                        // If at max level, but not max ticks, then technically
                        // still can "level up", but only the ticks get refilled
                        // but the level itself must not be increased. Something
                        // is wrong with the semantics here...
                        if(_level > _level_max)
                                _level = _level_max;
                }
        }

        void tick_down() {
                // Don't decrease ticks at the bottom level.
                if(_level == 1) {
                        _ticks = _ticks_per_level;
                        return;
                }

                if(_ticks > 0) {
                        // Decrement the ticks if there are any.
                        --_ticks;
                } else {
                        // Reset the ticks and decrement the level
                        // if there are no ticks left.
                        _ticks = _ticks_per_level;
                        --_level;
                }
        }

        unsigned get_level() const { return _level; }
        unsigned get_ticks() const { return _ticks; }
        unsigned get_ticks_per_level() const { return _ticks_per_level; }
};

class upgrades {
        upgrade_atom _gun;
        upgrade_atom _rl;

public:
        upgrades(unsigned gun_lvl_max,
                        unsigned rl_lvl_max,
                        unsigned gun_upgrade_ammo,
                        unsigned rl_upgrade_ammo)
        : _gun(1, gun_upgrade_ammo, gun_lvl_max)
        , _rl(1, rl_upgrade_ammo, rl_lvl_max)
        {}

        bool can_upgrade_gun() const { return _gun.can_level_up(); }
        unsigned gun_lvl() const { return _gun.get_level(); }
        unsigned gun_ticks() const { return _gun.get_ticks(); }
        unsigned gun_ticks_per_level() const { return _gun.get_ticks_per_level(); }
        void upgrade_gun() { _gun.level_up(); }
        void tick_down_gun() { _gun.tick_down(); }

        bool can_upgrade_rl() const { return _rl.can_level_up(); }
        unsigned rl_lvl() const { return _rl.get_level(); }
        unsigned rl_ticks() const { return _rl.get_ticks(); }
        unsigned rl_ticks_per_level() const { return _rl.get_ticks_per_level(); }
        void upgrade_rl() { _rl.level_up(); }
        void tick_down_rl() { _rl.tick_down(); }
};

// Constructors.
// -------------

shared_ptr<orientation> create_orientation(double x, double y, double theta);
shared_ptr<bounds> create_bounds(double x_min, double y_min, double x_max, double y_max);
shared_ptr<coll_queue> create_coll_queue();
shared_ptr<upgrades> create_upgrades(
                unsigned gun_lvl_max,
                unsigned rl_lvl_max,
                unsigned gun_upgrade_ammo,
                unsigned rl_upgrade_ammo);
shared_ptr<wellness> create_wellness(double health, double shield);

}

#endif /* CMP_MISC_H_ */
