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

#ifndef UPGRADE_ATOM_H
#define UPGRADE_ATOM_H

namespace cmp {

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

}

#endif
