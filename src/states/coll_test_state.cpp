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
#include "../efwk2/sys/display.h"

#include "state.h"

class coll_test_state : public state {
        const res::resman& m_resman;
        const script::scriptman& m_sman;
        bool m_done;

        efwk::shape_circle m_shp1;
        efwk::orientation m_ori1;

        efwk::shape_circle m_shp2;
        efwk::orientation m_ori2;

        efwk::shape_compound<efwk::shape_circle, efwk::shape_square> m_shp3;
        efwk::orientation m_ori3;

        std::map<int, bool> m_keys;
        std::vector<efwk::point> m_points;

public:
        coll_test_state(const res::resman& resman, const script::scriptman& sman) :
                m_resman(resman),
                m_sman(sman),
                m_done(false),
                m_shp1(100), m_ori1(100, 100, 0),
                m_shp2(150), m_ori2(300, 100, 0),
                m_shp3(std::make_pair(efwk::shape_circle { 50 }, efwk::orientation { 0, 0, 0 }),
                       std::make_pair(efwk::shape_square { 70 }, efwk::orientation { 10, 10, 0 })),
                m_ori3(200, 200, 0),
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

                m_ori3.shift(dx * dt * 100, dy * dt * 100);

                // Collision detection.
                m_points.clear();
                auto inserter = std::back_inserter(m_points);
                collide_impl(m_shp1, m_ori1, m_shp2, m_ori2, inserter);
                collide_impl(m_shp1, m_ori1, m_shp3, m_ori3, inserter);
                collide_impl(m_shp2, m_ori2, m_shp3, m_ori3, inserter);
        }

        void draw(double weight)
        {
                al_clear_to_color(al_map_rgb_f(0.333, 0.5, 0.667));

                // Draw shapes.
                display_shape(m_ori1, m_shp1);
                display_shape(m_ori2, m_shp2);
                display_shape(m_ori3, m_shp3);

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
