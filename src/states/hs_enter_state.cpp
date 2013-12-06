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

#include "../misc/high_score.h"
#include "state.h"

class hs_enter_state : public state {

        // External dependencies.
        // ----------------------
        res::resman const& _resman;
        script::scriptman const& _sman;
        const unsigned _score;

        // State.
        // ------
        std::string _current_name;
        bool _done;

        std::string current_to_print() const {
                std::string result = "typename : ";
                result += _current_name;
                return result;
        }

        void record_score() {
                high_score hs("highscore.txt");
                hs.try_adding_entry({ _current_name, _score });
        }

public:
        hs_enter_state(res::resman const& resman,
                       script::scriptman const& sman,
                       unsigned score)
        : _resman(resman)
        , _sman(sman)
        , _score(score)
        , _current_name("")
        {
                high_score hs("highscore.txt");
                _done = !hs.can_add_score(score);
        }

        void sigkill() {
                _done = true;
        }

        bool done() {
                return _done;
        }

        std::unique_ptr<state> next_state() {
                return create_hs_display_state(_resman, _sman);
        }

        void update(double t, double dt) {
        }

        void draw(double weight) {
                // Clear screen
                al_clear_to_color(al_map_rgb_f(
                        cfg::real("gfx_background_r"),
                        cfg::real("gfx_background_g"),
                        cfg::real("gfx_background_b")));

                // Prepare the resources.
                ALLEGRO_FONT const* font = _resman.get_font(res::res_id::FONT);
                const std::string to_print = current_to_print();

                // Determine the required dimensions.
                int bbx, bby, bbw, bbh;
                al_get_text_dimensions(
                        font, to_print.c_str(),
                        &bbx, &bby, &bbw, &bbh);

                const double scr_w = cfg::integer("gfx_screen_w");                
                const double scr_h = cfg::integer("gfx_screen_h");                

                // Place the text in the screen.
                const double x = (scr_w - static_cast<double>(bbw)) * 0.5;
                const double y = (scr_h - static_cast<double>(bbh)) * 0.5;

                const auto color = al_map_rgb_f(1, 1, 1);

                al_draw_text(font, color, x, y, 0, to_print.c_str());
        }

        void key_up(int k) {
                switch(k) {
                case ALLEGRO_KEY_BACKSPACE:
                        if(!_current_name.empty())
                                _current_name.pop_back();
                        break;

                case ALLEGRO_KEY_ENTER:
                        record_score();
                        _done = true;
                        break;

                case ALLEGRO_KEY_ESCAPE:
                        _done = true;
                        break;

                default:
                        break;
                }
        }

        void key_typed(int chr) {
                if(!isalnum(chr)) return;
                _current_name += static_cast<char>(chr);
        }
};

std::unique_ptr<state> create_hs_enter_state(
                res::resman const& res,
                script::scriptman const& sman,
                unsigned score) {
        return std::unique_ptr<state>(new hs_enter_state(res, sman, score));
}
