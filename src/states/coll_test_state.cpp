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

#include <allegro5/allegro_primitives.h>

#include "../efwk2/cmp/shape.h"
#include "../efwk2/sys/collision.h"

// TODO: merge the display and the display dbg modules and have two operations
//       available inside.

#include "state.h"

class coll_test_state : public state {
        const res::resman& m_resman;
        const script::scriptman& m_sman;
        bool m_done;

        efwk::shape_circle m_cir1;
        double m_x1, m_y1;

        efwk::shape_circle m_cir2;
        double m_x2, m_y2;

        std::map<int, bool> m_keys;
        std::vector<efwk::point> m_points;

public:
        coll_test_state(const res::resman& resman, const script::scriptman& sman) :
                m_resman(resman),
                m_sman(sman),
                m_done(false),
                m_cir1(100),
                m_x1(100), m_y1(100),
                m_cir2(150),
                m_x2(300), m_y2(100),
                m_keys({
                        { ALLEGRO_KEY_UP, false },
                        { ALLEGRO_KEY_RIGHT, false },
                        { ALLEGRO_KEY_DOWN, false },
                        { ALLEGRO_KEY_LEFT, false }
                })
        {
        }

        void sigkill()
        {
                m_done = true;
        }

        bool done()
        {
                return m_done;
        }

        std::unique_ptr<state> next_state()
        {
                return create_menu_state(m_resman, m_sman);
        }

        void update(double t, double dt)
        {
                // Shape movement.
                double dx = 0;
                dx -= m_keys[ALLEGRO_KEY_LEFT];
                dx += m_keys[ALLEGRO_KEY_RIGHT];

                double dy = 0;
                dy -= m_keys[ALLEGRO_KEY_UP];
                dy += m_keys[ALLEGRO_KEY_DOWN];

                m_x2 += dx * dt * 100;
                m_y2 += dy * dt * 100;

                // Collision detection.
                m_points.clear();
                auto inserter = std::back_inserter(m_points);
                collide_impl(m_cir1, efwk::orientation { m_x1, m_y1, 0 },
                             m_cir2, efwk::orientation { m_x2, m_y2, 0 },
                             inserter);
        }

        void draw(double weight)
        {
                al_clear_to_color(al_map_rgb_f(0.333, 0.5, 0.667));

                // Draw shapes.
                al_draw_circle(m_x1, m_y1, m_cir1.radius, al_map_rgb_f(1, 1, 1), 1);
                al_draw_circle(m_x2, m_y2, m_cir2.radius, al_map_rgb_f(1, 1, 1), 1);

                // Draw collision points.
                for (const auto& p : m_points) {
                        al_draw_circle(p.x, p.y, 5, al_map_rgb_f(1, 0, 0), 2);
                }
        }

        void key_up(int k)
        {
                if (k == ALLEGRO_KEY_ESCAPE)
                        m_done = true;

                m_keys[k] = false;
        }

        void key_down(int k)
        {
                m_keys[k] = true;
        }
};

std::unique_ptr<state> create_coll_test_state(
                res::resman const& resman,
                script::scriptman const& sman)
{
        return std::unique_ptr<state>(new coll_test_state(resman, sman));
}
