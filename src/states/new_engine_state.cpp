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

#include "../efwk2/sys/bounding.h"
#include "../efwk2/sys/display.h"
#include "../efwk2/sys/display_dbg.h"
#include "../efwk2/sys/movement.h"
#include "../efwk2/sys/input.h"
#include "../efwk2/sys/collision.h"
#include "../efwk2/sys/pain.h"
#include "../efwk2/sys/hud.h"

#include "../efwk2/tmp/map.h"
#include "../efwk2/comm.h"

#include "../gameplay2/ent_player.h"
#include "../gameplay2/ent_enemy.h"
#include "../gameplay2/ent_bullet.h"

#include "../misc/logger.h"
#include "../misc/config.h"

namespace
{

// Generic operations on the whole entities.
// -----------------------------------------

class pre_collision_update_func
{
        const std::map<int, bool>& m_keys;
        const res::resman& m_resman;
        efwk::comm_bus& m_cbus;

        double m_dt;

public:
        pre_collision_update_func(const std::map<int, bool>& keys,
                                  const res::resman& resman,
                                  efwk::comm_bus& cbus,
                                  double dt) :
                m_keys(keys),
                m_resman(resman),
                m_cbus(cbus),
                m_dt(dt)
        {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                efwk::weapon_input(ent, m_keys, m_dt, m_cbus);
                efwk::move(ent, m_dt);
                efwk::bind_movement(ent);
                efwk::bind_life(ent, m_cbus);
        }
};

struct collq_clear_func
{
        template <class Entity>
        void operator()(Entity& ent)
        {
                ent.collq.clear();
        }
};

struct collide_func
{
        template <class First, class Second>
        void operator()(First& first, Second& second)
        {
                efwk::check_collisions(first, second);
        }
};

class post_collision_update_func
{
        efwk::comm_bus& m_cbus;

public:
        post_collision_update_func(efwk::comm_bus& cbus) : m_cbus(cbus) {}

        template <class Entity>
        void operator()(Entity& ent)
        {
                efwk::pain(ent, m_cbus);
        }
};

class handle_event_func
{
        gplay::player& m_player;
        std::vector<gplay::bullet>& m_bullets;
        std::vector<gplay::enemy>& m_enemies;

        int& m_player_score;

public:
        handle_event_func(gplay::player& player,
                          std::vector<gplay::bullet>& bullets,
                          std::vector<gplay::enemy>& enemies,
                          int& player_score) :
                m_player(player),
                m_bullets(bullets),
                m_enemies(enemies),
                m_player_score(player_score)
        {
        }

        void operator()(const efwk::death_event& ev)
        {
                if (ev.score_id == m_player.id) {
                        m_player_score += 10;
                }
        }
};

class try_remove_func
{
        long m_id;

public:
        try_remove_func(long id) : m_id(id) {}

        template <class Entity>
        bool operator()(std::vector<Entity>& col)
        {
                auto found = std::find_if(
                        begin(col), end(col),
                        [this](const Entity& ent)
                        {
                                return ent.id == m_id;
                        });

                if (found == end(col))
                        return false;

                *found = std::move(col.back());
                col.pop_back();
                return true;
        }
};

class draw_func
{
        bool m_debug_key;
        double m_weight;
        ALLEGRO_FONT* m_debug_font;

public:
        draw_func(bool debug_key, double weight, ALLEGRO_FONT* debug_font) :
                m_debug_key(debug_key),
                m_weight(weight),
                m_debug_font(debug_font)
        {}

        template <class Entity>
        void operator()(const Entity& ent)
        {
                efwk::display(ent, m_weight);
                if (m_debug_key)
                        efwk::display_dbg(ent, m_weight, m_debug_font);
        }
};

}

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
        std::vector<gplay::enemy> m_enemies;

        int m_player_score;

        efwk::comm_bus m_cbus;

        long m_get_next_id()
        {
                return ++m_next_id;
        }

        void m_log_state()
        {
                std::stringstream ss;

                ss << "Frame logic state:" << std::endl;
                ss << "Bullets count: " << m_bullets.size() << "." << std::endl;
                ss << "Enemies count: " << m_enemies.size() << ".";

                logger::instance().trace(ss.str());
        }

        void m_update_entities(double dt)
        {
                pre_collision_update_func precuf { m_keys, m_resman, m_cbus, dt };
                efwk::map(precuf, m_player, m_bullets, m_enemies);

                efwk::map(collq_clear_func(), m_player, m_bullets, m_enemies);
                efwk::map2(collide_func(), m_player, m_bullets, m_enemies);

                post_collision_update_func postcuf { m_cbus };
                efwk::map(postcuf, m_player, m_bullets, m_enemies);
        }

        void m_handle_events()
        {
                handle_event_func hef { m_player, m_bullets, m_enemies, m_player_score };
                efwk::map(hef, m_cbus.death_events);
                m_cbus.clear_events();
        }

        void m_handle_deletions(double dt)
        {
                m_cbus.del_reqs.visit(dt, [this](long rem_id) {

                        if (rem_id == m_player.id)
                                throw; // TODO: enable state end here.

                        try_remove_func trf { rem_id };
                        efwk::try_each(trf, m_bullets, m_enemies);
                });
        }

        void m_handle_creations(double dt)
        {
                // Handle creation messages.
                m_cbus.bullet_reqs.visit(dt, [this](efwk::bullet_req& brq) {
                        m_bullets.emplace_back(
                                m_get_next_id(),
                                m_player.id,
                                m_resman.get_bitmap(res::res_id::BULLET_5),
                                800.0, brq.vx, brq.vy,
                                brq.x, brq.y, -3.1415 / 2.0,
                                0, 0,
                                cfg::integer("gfx_screen_w"),
                                cfg::integer("gfx_screen_h"),
                                5.0,
                                brq.is_enemy,
                                brq.damage);
                });

                // Spawn enemy.
                if ((m_next_enemy_counter -= dt) > 0) {
                        return;
                }

                m_next_enemy_counter += 3.0;
                m_enemies.emplace_back(
                        m_get_next_id(),
                        m_resman.get_bitmap(res::res_id::ENEMY_LIGHT_FIGHTER),
                        100.0,
                        400.0, 10.0,
                        0, 0, 
                        cfg::integer("gfx_screen_w"),
                        cfg::integer("gfx_screen_h"),
                        32.0,
                        100.0,
                        50.0);
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
                        m_player(m_get_next_id(),
                                m_resman.get_bitmap(res::res_id::PLAYER_SHIP),
                                400.0, m_keys,
                                100.0, 100.0, -3.1415 * 0.5,
                                0.0, 0.0, m_screen_w, m_screen_h,
                                0.1, 1.0,
                                24.0,
                                100.0,
                                100.0),
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
                m_log_state();
                m_update_entities(dt);
                m_handle_events();
                m_handle_deletions(dt);
                m_handle_creations(dt);
                logger::instance().flush();
        }

        void draw(double weight) override
        {
                al_clear_to_color(al_map_rgb_f(0, 0, 0));
                draw_func df { m_keys.at(ALLEGRO_KEY_SPACE), weight, m_debug_font };
                efwk::map(df, m_player, m_bullets, m_enemies);
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
        return std::unique_ptr<state>(new new_engine_state(resman, sman));
}
