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

#include <algorithm>

#include "../misc/high_score.h"
#include "state.h"

class hs_display_state : public state {

        // Configuration.
        // --------------
        res::resman const& _resman;
        script::scriptman const& _sman;
        const double _vmargin;
        const unsigned _hs_char_width;

        // Precomputations.
        // ----------------
        const double _screen_w, _screen_h;
        std::vector<std::string> _strings;
        double _hs_pix_width;
        double _hs_pix_height;
        double _hs_line_pix_height;

        // State.
        // ------
        bool _done;

        // Helpers.
        // --------
        void entry_str(high_score::entry const& e,
                        std::string& name_str,
                        std::string& score_str) {

                std::stringstream ss;
                ss << e.score;

                name_str = e.name;
                score_str = ss.str();
        }

public:
        hs_display_state(res::resman const& resman,
                         script::scriptman const& sman)
        : _resman(resman)
        , _sman(sman)
        , _vmargin(20.0)
        , _hs_char_width(30)
        , _screen_w(cfg::integer("gfx_screen_w"))
        , _screen_h(cfg::integer("gfx_screen_h"))
        , _done(false)
        {
                // Read the high score entries.
                high_score hs("highscore.txt");
                auto entries = hs.get_entries();

                // Build the score strings.
                for(auto const& e : entries) {

                        std::string name_str;
                        std::string score_str;
                        entry_str(e, name_str, score_str);
                        
                        const unsigned ns = name_str.size();
                        const unsigned ss = score_str.size();
                        const unsigned num_dots = _hs_char_width - ns - ss;

                        std::string entry_str = name_str;
                        entry_str.append(num_dots, '.');
                        entry_str.append(score_str);

                        _strings.push_back(move(entry_str));
                }

                // Measure the pixel width.
                ALLEGRO_FONT const* font = _resman.get_font(res::res_id::FONT);
                int bbx, bby, bbw, bbh;
                al_get_text_dimensions(
                                font, _strings.front().c_str(),
                                &bbx, &bby, &bbw, &bbh);
                _hs_pix_width = bbw;
                _hs_line_pix_height = bbh;
                _hs_pix_height =
                        bbh * _strings.size() + 
                        _vmargin * (_strings.size() - 1);
        }

        void sigkill() {
                _done = true;
        }

        bool done() {
                return _done;
        }

        std::unique_ptr<state> next_state() {
                return create_menu_state(_resman, _sman);
        }

        // Logic.
        void update(double t, double dt) {
        }

        void draw(double weight) {

                al_clear_to_color(al_map_rgb_f(0, 0, 0));

                ALLEGRO_FONT const* font = _resman.get_font(res::res_id::FONT);

                const double left = (_screen_w - _hs_pix_width) * 0.5;
                const auto color = al_map_rgb_f(1, 1, 1);

                double current_y = (_screen_h - _hs_pix_height) * 0.5;

                for(auto const& s : _strings) {
                        al_draw_text(
                                font,
                                color,
                                left,
                                current_y,
                                0,
                                s.c_str());

                        current_y += _hs_line_pix_height + _vmargin;
                }
        }

        // Optionally implementable.
        void key_up(int k) {
                _done = true;
        }
};

std::unique_ptr<state> create_hs_display_state(
                res::resman const& res,
                script::scriptman const& sman) {

        return std::unique_ptr<state>(new hs_display_state(res, sman));
}

