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

#ifndef STATE_H
#define STATE_H

#include <memory>

#include "../misc/config.h"
#include "../resources/resman.h"
#include "../script/scriptman.h"

class resources;

class state {
public:
        virtual ~state() {}

        // State transitions related.
        virtual void sigkill() = 0;
        virtual bool done() = 0;
        virtual unique_ptr<state> next_state() = 0;

        // Logic.
        virtual void frame_logic(double dt) = 0;

        // Optionally implementable.
        virtual void key_up(int k) {}
        virtual void key_down(int k) {}
};

std::unique_ptr<state> create_menu_state(
                res::resman const& res,
                script::scriptman const& sman);

std::unique_ptr<state> create_game_state(
                res::resman const& res,
                script::scriptman const& sman);

#endif
