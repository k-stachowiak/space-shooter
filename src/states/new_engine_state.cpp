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

#include "state.h"

#include "../gameplay2/game.h"

class new_engine_state: public state
{
        bool m_done;
        res::resman const& m_resman;
        script::scriptman const& m_sman;

        std::map<int, bool> m_keys;

        gplay::game m_game;

public:
        new_engine_state(
                res::resman const& resman,
                script::scriptman const& sman) :
                        m_done(false),
                        m_resman(resman),
                        m_sman(sman),
                        m_game(resman, m_keys)
        {
                m_keys[ALLEGRO_KEY_LEFT] = false;
                m_keys[ALLEGRO_KEY_UP] = false;
                m_keys[ALLEGRO_KEY_RIGHT] = false;
                m_keys[ALLEGRO_KEY_DOWN] = false;
                m_keys[ALLEGRO_KEY_Z] = false;
                m_keys[ALLEGRO_KEY_X] = false;
        }

        void sigkill() override
        {
                m_done = true;
        }

        bool done() override
        {
                return m_done;
        }

        std::unique_ptr<state> next_state() override
        {
                return create_menu_state(m_resman, m_sman);
        }

        void update(double t, double dt) override
        {
                m_game.update(dt);
        }

        void draw(double weight) override
        {
                m_game.draw(weight);
        }

        void key_down(int k) override
        {
                m_keys[k] = true;
        }

        void key_up(int k) override
        {
                m_keys[k] = false;
                if (k == ALLEGRO_KEY_ESCAPE)
                        m_done = true;
        }
};

std::unique_ptr<state> create_new_engine_state(
                res::resman const& resman,
                script::scriptman const& sman)
{
        return std::unique_ptr<state>(new new_engine_state(resman, sman));
}
