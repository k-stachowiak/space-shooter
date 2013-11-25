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

#ifndef MENU_ENT_FAC_H
#define MENU_ENT_FAC_H

class menu_entity_factory {

        uint64_t _last_id;

        res::resman const& _resman;

        sys::movement_system&    _movement_system;
        sys::fx_system&          _fx_system;
        sys::drawing_system&     _drawing_system;
        sys::sound_system&       _sound_system;

public:
        menu_entity_factory(
                res::resman const& rm,
                sys::movement_system& movement_system,
                sys::fx_system& fx_system,
                sys::drawing_system& drawing_system,
                sys::sound_system& sound_system)
        : _last_id(0)
        , _resman(rm)
        , _movement_system(movement_system)
        , _fx_system(fx_system)
        , _drawing_system(drawing_system)
        , _sound_system(sound_system)
        {}

        uint64_t create_menu_entry(double x, double y, string text);
};

#endif
