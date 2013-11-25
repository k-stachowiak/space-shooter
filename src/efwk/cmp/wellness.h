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

#ifndef WELLNESS_H
#define WELLNESS_H

#include <memory>
#include <stdint.h>

namespace cmp {

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

    std::shared_ptr<wellness> create_wellness(double health, double shield);

}

#endif
