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

#include "../efwk2/tmp/map.h"
#include "../efwk2/sys/complex.h"
#include "../efwk2/comm.h"

#include "../gameplay2/ent_player.h"
#include "../gameplay2/ent_enemy.h"
#include "../gameplay2/ent_spark.h"
#include "../gameplay2/ent_projectile.h"
#include "../gameplay2/ent_anim_sprite.h"
#include "../gameplay2/construct.h"

#include "../misc/logger.h"
#include "../misc/config.h"

// TODO:
// - shooting enemies

class new_engine_state: public state
{
        bool m_done;
        res::resman const& m_resman;
        script::scriptman const& m_sman;

        const gplay::entity_constructor m_econstr;

        const double m_screen_w;
        const double m_screen_h;

        std::map<int, bool> m_keys;

        ALLEGRO_FONT* const m_debug_font;
        ALLEGRO_FONT* const m_score_font;

        long m_next_id;

        double m_next_lfighter_counter;
        double m_next_lbomber_counter;

        gplay::player m_player;

        std::vector<gplay::projectile> m_projectiles;
        std::vector<gplay::spark> m_sparks;
        std::vector<gplay::pickup> m_pickups;
        std::vector<gplay::enemy> m_enemies;
        std::vector<gplay::anim_sprite> m_sprites;

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
                ss << "Projectiles count: " << m_projectiles.size() << "." << std::endl;
                ss << "Sparks count: " << m_sparks.size() << "." << std::endl;
                ss << "Enemies count: " << m_enemies.size() << ".";
                logger::instance().trace(ss.str());
                logger::instance().flush();
        }
        */

        void m_update_entities(double dt)
        {
                efwk::pre_collision_update_func pre_coll { m_keys, m_resman, m_cbus, dt };
                efwk::map(pre_coll, m_player,
                                    m_projectiles,
                                    m_sparks,
                                    m_pickups,
                                    m_enemies,
                                    m_sprites);

                efwk::collide_func coll;
                efwk::map2(coll, m_player,
                                 m_projectiles,
                                 m_pickups,
                                 m_enemies);

                efwk::post_collision_update_func post_coll { m_cbus, dt };
                efwk::map(post_coll, m_player,
                                     m_projectiles,
                                     m_sparks,
                                     m_pickups,
                                     m_enemies,
                                     m_sprites);
        }

        void m_handle_events()
        {
                efwk::handle_event_func hef { m_player.id, m_player.score };
                efwk::map(hef, m_cbus.death_events);
                m_cbus.clear_events();
        }

        void m_handle_deletions(double dt)
        {
                m_cbus.del_reqs.visit(dt, [this](long rem_id) {

                        if (m_done || rem_id == m_player.id) {
                                m_done = true;
                                return;
                        }

                        efwk::try_remove_func trf { rem_id };
                        efwk::try_each(trf, m_projectiles,
                                            m_sparks,
                                            m_pickups,
                                            m_enemies,
                                            m_sprites);
                });
        }

        void m_handle_creations(double dt)
        {
                // Handle creation messages.
                m_cbus.bullet_reqs.visit(dt, [this](efwk::bullet_req& brq) {
                        m_projectiles.push_back(
                                m_econstr.make_bullet(
                                        m_get_next_id(),
                                        m_player.id,
                                        brq.x, brq.y,
                                        brq.is_enemy,
                                        brq.damage));
                });

                m_cbus.missile_reqs.visit(dt, [this](efwk::missile_req& mrq) {
                        m_projectiles.push_back(
                                m_econstr.make_missile(
                                        m_get_next_id(),
                                        m_player.id,
                                        mrq.x, mrq.y,
                                        mrq.is_enemy,
                                        mrq.damage));
                });

                m_cbus.spark_reqs.visit(dt, [this](efwk::spark_req& srq) {
                        m_sparks.push_back(
                                m_econstr.make_spark(
                                        m_get_next_id(),
                                        srq.ttl,
                                        srq.rgb,
                                        srq.x, srq.y,
                                        srq.vx, srq.vy));
                });

                m_cbus.health_reqs.visit(dt, [this](efwk::pickup_req& prq) {
                        m_pickups.push_back(
                                m_econstr.make_health_pickup(
                                        m_get_next_id(),
                                        prq.parent_id,
                                        prq.x, prq.y,
                                        prq.vx, prq.vy));
                });

                m_cbus.bullupgr_reqs.visit(dt, [this](efwk::pickup_req& prq) {
                        m_pickups.push_back(
                                m_econstr.make_bullupgr_pickup(
                                        m_get_next_id(),
                                        prq.parent_id,
                                        prq.x, prq.y,
                                        prq.vx, prq.vy));
                });

                m_cbus.missupgr_reqs.visit(dt, [this](efwk::pickup_req& prq) {
                        m_pickups.push_back(
                                m_econstr.make_missupgr_pickup(
                                        m_get_next_id(),
                                        prq.parent_id,
                                        prq.x, prq.y,
                                        prq.vx, prq.vy));
                });

                m_cbus.expl_reqs.visit(dt, [this](efwk::explosion_req& erq) {
                        m_sprites.push_back(m_econstr.make_explosion(m_get_next_id(), erq.x, erq.y));
                });

                m_cbus.smoke_reqs.visit(dt, [this](efwk::smoke_req& srq) {
                        m_sprites.push_back(m_econstr.make_smoke_small(m_get_next_id(), srq.x, srq.y));
                });

                // Spawn enemies.
                // --------------

                if ((m_next_lfighter_counter -= dt) <= 0) {
                        m_next_lfighter_counter += 3.0;
                        m_enemies.push_back(
                                m_econstr.make_light_fighter(
                                        m_get_next_id(),
                                        300.0, 10.0));
                }

                if ((m_next_lbomber_counter -= dt) <= 0) {
                        m_next_lbomber_counter += 3.0;
                        m_enemies.push_back(
                                m_econstr.make_light_bomber(
                                        m_get_next_id(),
                                        500.0, 10.0));
                }
        }

public:
        new_engine_state(
                res::resman const& resman,
                script::scriptman const& sman) :
                        m_done(false),
                        m_resman(resman),
                        m_sman(sman),
                        m_econstr(resman),
                        m_screen_w(cfg::integer("gfx_screen_w")),
                        m_screen_h(cfg::integer("gfx_screen_h")),
                        m_debug_font(m_resman.get_font(res::res_id::TINY_FONT)),
                        m_score_font(m_resman.get_font(res::res_id::FONT)),
                        m_next_id(0),
                        m_next_lfighter_counter(0),
                        m_next_lbomber_counter(0),
                        m_player(m_econstr.make_player(m_get_next_id(), m_keys, 100.0, 100.0))
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
                const bool debug_mode = m_keys[ALLEGRO_KEY_SPACE];

                al_clear_to_color(al_map_rgb_f(0, 0, 0));

                efwk::draw_func df { debug_mode, weight, m_debug_font };
                efwk::map(df, m_player, m_projectiles, m_enemies, m_sprites, m_sparks, m_pickups);
                efwk::draw_hud(m_score_font,
                               m_player.score,
                               m_player.wlns.get_health(),
                               m_player.upgr.get_gun_level(),
                               m_player.upgr.get_missile_level());
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
