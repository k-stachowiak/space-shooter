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

#include <vector>
#include <string>

#include "../efwk/sys/systems.h"
#include "state.h"

using namespace std;

class menu_state : public state {

        // External dependencies.
        // ----------------------
        res::resman const& _resman;
        script::scriptman const& _sman;

        // State.
        // ------
        bool _done;
        unique_ptr<state> _next_state;
        vector<string> _entries;
        unsigned _current_entry;
        comm::msg_queue _messages;

        // Systems.
        // --------
        sys::movement_system    _msys;
        sys::fx_system          _fxsys;
        sys::drawing_system     _dsys;
        sys::input_system       _isys;
        sys::sound_system       _ssys;

        // State operations.
        // -----------------
        void menu_up() {
                if(!_current_entry)
                        _current_entry = _entries.size() - 1;
                else
                        --_current_entry;
        }

        void menu_down() {
                ++_current_entry;
                if(_current_entry >= _entries.size())
                        _current_entry = 0;
        }

        void menu_action() {

                string const& entry = _entries[_current_entry];
        
                if(entry == "game") 
                        _next_state = create_game_state(_resman, _sman);

                else if(entry == "hoghscore") 
                        //_next_state = create_highscore_state(_resman, _sman);
                        _next_state.reset();

                else if(entry == "quit") 
                        _next_state.reset();

                else 
                        throw fatal_error("Menu entries inconsistent.");

                _done = true;
        }

public:
        menu_state(const res::resman& resman, script::scriptman const& sman)
        : _resman(resman)
        , _sman(sman)
        , _done(false)
        , _entries(vector<string> { "game", "highscore", "quit" })
        , _current_entry(0)
        , _dsys(resman.get_font(res::res_id::TINY_FONT))
        , _ssys(_resman)
        {}

        void sigkill() {
                _done = true;
        }

        bool done() {
                return _done;
        }

        unique_ptr<state> next_state() {
                return move(_next_state);
        }

        void frame_logic(double dt) {
                _msys.update(dt, _messages);
                _fxsys.update(dt, _messages);
                _dsys.update(dt);
                _isys.update();
                _ssys.update(dt);
        }

        void key_up(int k) {
                switch(k) {
                case ALLEGRO_KEY_UP:
                        menu_up();
                        break;

                case ALLEGRO_KEY_DOWN:
                        menu_down();
                        break;

                case ALLEGRO_KEY_ENTER:
                        menu_action();
                        break;

                default:
                        break;
                }
        }
};

unique_ptr<state> create_menu_state(
                res::resman const& res,
                script::scriptman const& sman) {

        return unique_ptr<state>(new menu_state(res, sman));
}
