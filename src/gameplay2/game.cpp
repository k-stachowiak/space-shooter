/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#include "game.h"

#include "../misc/config.h"

namespace gplay
{

game::game(const std::map<int, bool>& keys) :
        m_screen_w(cfg::integer("gfx_screen_w")),
        m_screen_h(cfg::integer("gfx_screen_h")),
        m_keys(keys),
        m_player("data/player_ship.png",
                500.0, m_keys,
                100.0, 100.0, -3.1415 * 0.5,
                0.0, 0.0, m_screen_w, m_screen_h)
{
}

void game::update(double dt)
{
        efwk::weapon_input(m_player, m_keys);
        efwk::move_ent(m_player, dt);
        efwk::bind_movement(m_player);
}

void game::draw(double weight)
{
        al_clear_to_color(al_map_rgb_f(0, 0, 0));
        efwk::display_ent(m_player, weight);
}

}
