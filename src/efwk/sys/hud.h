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

#ifndef SYS_HUD_H
#define SYS_HUD_H

#include <vector>

#include "base.h"
#include "nodes.h"

namespace sys {

class hud_system : public system {

        ALLEGRO_BITMAP* _hud_bg;
        ALLEGRO_BITMAP* _hud_health;
        ALLEGRO_BITMAP* _hud_battery;
        ALLEGRO_BITMAP* _hud_diode_on;
        ALLEGRO_BITMAP* _hud_diode_off;
        ALLEGRO_BITMAP* _b_upgrade;
        ALLEGRO_BITMAP* _m_upgrade;

        ALLEGRO_FONT* _score_font;
        ALLEGRO_FONT* _hud_font;
        double _screen_w, _screen_h;

        std::vector<nd::hud_node> _nodes;

        void draw_background();
        void draw_score(nd::hud_node const& n);
        void draw_wellness(nd::hud_node const& n);
        void draw_upgrades(nd::hud_node const& n);
        void draw_ammo(nd::hud_node const& n);

public:
        hud_system(
                        ALLEGRO_BITMAP* hud_bg,
                        ALLEGRO_BITMAP* hud_health,
                        ALLEGRO_BITMAP* hud_battery,
                        ALLEGRO_BITMAP* hud_diode_on,
                        ALLEGRO_BITMAP* hud_diode_off,
                        ALLEGRO_BITMAP* b_upgrade,
                        ALLEGRO_BITMAP* m_upgrade,
                        ALLEGRO_FONT* score_font,
                        ALLEGRO_FONT* hud_font,
                        double screen_w,
                        double screen_h)
        : _hud_bg        (hud_bg)
        , _hud_health    (hud_health)
        , _hud_battery   (hud_battery)
        , _hud_diode_on  (hud_diode_on)
        , _hud_diode_off (hud_diode_off)
        , _b_upgrade     (b_upgrade)
        , _m_upgrade     (m_upgrade)
        , _score_font    (score_font)
        , _hud_font      (hud_font)
        , _screen_w      (screen_w)
        , _screen_h      (screen_h)
        {}
        void remove_node(uint64_t id) { remove_nodes(_nodes, id); }
        unsigned num_nodes() const { return _nodes.size(); }
        void add_node(nd::hud_node node) { _nodes.push_back(node); }
        void update();
};

}

#endif
