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

// TODO: create a test state for the collisions test between different shapes...
//       then the sparks should almost work (negative velocities aren't generated
//       by the current formula).

#include "state.h"

#include "../efwk2/tmp/map.h"
#include "../efwk2/comm.h"

#include "../gameplay2/ent_player.h"
#include "../gameplay2/ent_enemy.h"
#include "../gameplay2/ent_spark.h"
#include "../gameplay2/ent_bullet.h"
#include "../gameplay2/logic.h"
#include "../gameplay2/construct.h"

#include "../misc/logger.h"
#include "../misc/config.h"

class new_engine_state: public state
{
        bool m_done;
        res::resman const& m_resman;
        script::scriptman const& m_sman;

        const double m_screen_w;
        const double m_screen_h;

        std::map<int, bool> m_keys;

        ALLEGRO_FONT* const m_debug_font;
        ALLEGRO_FONT* const m_score_font;

        long m_next_id;

        double m_next_enemy_counter;

        gplay::player m_player;
        std::vector<gplay::bullet> m_bullets;
        std::vector<gplay::spark> m_sparks;
        std::vector<gplay::enemy> m_enemies;

        int m_player_score;

        efwk::comm_bus m_cbus;

        long m_get_next_id()
        {
                return ++m_next_id;
        }

        /*
        void m_log_state()
        {
                std::stringstream ss;
                ss << "Frame logic state:" << std::endl;
                ss << "Bullets count: " << m_bullets.size() << "." << std::endl;
                ss << "Sparks count: " << m_sparks.size() << "." << std::endl;
                ss << "Enemies count: " << m_enemies.size() << ".";
                logger::instance().trace(ss.str());
                logger::instance().flush();
        }
        */

        void m_update_entities(double dt)
        {
                gplay::pre_collision_update_func precuf { m_keys, m_resman, m_cbus, dt };
                efwk::map(precuf, m_player, m_bullets, m_sparks, m_enemies);

                gplay::collq_clear_func ccf;
                efwk::map(ccf, m_player, m_bullets, m_enemies);
                gplay::collide_func cf;
                efwk::map2(cf, m_player, m_bullets, m_enemies);

                gplay::post_collision_update_func postcuf { m_cbus };
                efwk::map(postcuf, m_player, m_bullets, m_sparks, m_enemies);
        }

        void m_handle_events()
        {
                gplay::handle_event_func hef { m_player, m_player_score };
                efwk::map(hef, m_cbus.death_events);
                m_cbus.clear_events();
        }

        void m_handle_deletions(double dt)
        {
                m_cbus.del_reqs.visit(dt, [this](long rem_id) {

                        if (rem_id == m_player.id)
                                throw; // TODO: enable state end here.

                        gplay::try_remove_func trf { rem_id };
                        efwk::try_each(trf, m_bullets, m_sparks, m_enemies);
                });
        }

        void m_handle_creations(double dt)
        {
                // Handle creation messages.
                m_cbus.bullet_reqs.visit(dt, [this](efwk::bullet_req& brq) {
                        m_bullets.push_back(
                                gplay::make_bullet(
                                        m_get_next_id(),
                                        m_player.id,
                                        brq.x, brq.y,
                                        brq.is_enemy,
                                        brq.damage,
                                        m_resman.get_bitmap(res::res_id::BULLET_5)));
                });

                m_cbus.spark_reqs.visit(dt, [this](efwk::spark_req& srq) {
                        m_sparks.push_back(
                                gplay::make_spark(
                                        m_get_next_id(),
                                        srq.ttl,
                                        srq.rgb,
                                        srq.x, srq.y,
                                        srq.vx, srq.vy));
                });

                // Spawn enemy.
                if ((m_next_enemy_counter -= dt) > 0) {
                        return;
                }

                m_next_enemy_counter += 3.0;
                m_enemies.push_back(
                        gplay::make_light_fighter(
                                m_get_next_id(),
                                m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER),
                                400.0, 10.0));
        }

public:
        new_engine_state(
                res::resman const& resman,
                script::scriptman const& sman) :
                        m_done(false),
                        m_resman(resman),
                        m_sman(sman),
                        m_screen_w(cfg::integer("gfx_screen_w")),
                        m_screen_h(cfg::integer("gfx_screen_h")),
                        m_debug_font(m_resman.get_font(res::res_id::TINY_FONT)),
                        m_score_font(m_resman.get_font(res::res_id::FONT)),
                        m_next_id(0),
                        m_next_enemy_counter(0),
                        m_player(
                                gplay::make_player(
                                        m_get_next_id(),
                                        m_resman.get_bitmap(res::res_id::PLAYER_SHIP),
                                        m_keys,
                                        100.0, 100.0)),
                        m_player_score(0)
        {
                m_keys[ALLEGRO_KEY_LEFT] = false;
                m_keys[ALLEGRO_KEY_UP] = false;
                m_keys[ALLEGRO_KEY_RIGHT] = false;
                m_keys[ALLEGRO_KEY_DOWN] = false;
                m_keys[ALLEGRO_KEY_Z] = false;
                m_keys[ALLEGRO_KEY_X] = false;
                m_keys[ALLEGRO_KEY_SPACE] = false;
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
                // m_log_state();
                m_update_entities(dt);
                m_handle_events();
                m_handle_deletions(dt);
                m_handle_creations(dt);
        }

        void draw(double weight) override
        {
                al_clear_to_color(al_map_rgb_f(0, 0, 0));
                gplay::draw_func df { m_keys.at(ALLEGRO_KEY_SPACE), weight, m_debug_font };
                efwk::map(df, m_player, m_bullets, m_sparks, m_enemies);
                efwk::draw_hud(m_score_font, m_player_score);
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
        return std::unique_ptr<state>(new new_engine_state { resman, sman });
}
