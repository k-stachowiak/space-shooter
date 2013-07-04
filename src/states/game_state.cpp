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

#include <functional>
#include <random>

#include "state.h"
#include "../efwk/game_ent_fac.h"
#include "../script/scriptman.h"
#include "../script/parsing.h"
#include "../misc/random_clock.h"
#include "../misc/rand.h"
#include "../efwk/sys/systems.h"
#include "../efwk/comm.h"
#include "../gameplay/enemy_manager.h"

#include <allegro5/allegro_primitives.h>

using namespace std;

static wave read_wave_from_script(script::dom_node const& wave_desc) {

        if(!is_list(wave_desc))
                throw parsing_error("Wave descriptor isn't a list.");

        vector<pair<double, pattern>> patterns;
        
        // Read the patterns
        for(script::dom_node const& ps : wave_desc.list) {

                if(!is_list(ps))
                        throw parsing_error("Pattern descriptor isn't a list.");

                if(!list_size(ps, 4))
                        throw parsing_error("Invalid list length encountered.");

                if(!all_atoms(ps)) {
                        throw parsing_error("Pattern must be defined "
                                        "as an all atoms list.");
                }

                // Parse delay.
                string delay_literal = ps.list[0].atom;
                bool delay_int;
                double ddelay;
                int idelay;
                if(!script::parse_literal(
                                delay_literal,
                                delay_int,
                                idelay, ddelay))
                        throw parsing_error("Failed parsing pattern delay.");

                double delay = delay_int
                        ? (double)idelay
                        : ddelay;

                // Parse the enemy type.
                string et_desc = ps.list[2].atom;
                enemy_type et;

                if(et_desc == "light_fighter")
                        et = enemy_type::light_fighter;

                else if(et_desc == "heavy_fighter")
                        et = enemy_type::heavy_fighter;

                else if(et_desc == "light_bomber")
                        et = enemy_type::light_bomber;

                else if(et_desc == "heavy_bomber")
                        et = enemy_type::heavy_bomber;

                else
                        throw parsing_error("unrecognized enemy type "
                                        "descriptor encountered.");
                
                // parse formation
                string formation_desc = ps.list[1].atom;
                vector<pattern::element> formation;

                if(formation_desc == "uno")
                        formation = pattern::el_uno(et);

                else if(formation_desc == "pair")
                        formation = pattern::el_pair(et);
                
                else if(formation_desc == "triangle")
                        formation = pattern::el_triangle(et);
                
                else if(formation_desc == "quad")
                        formation = pattern::el_quad(et);

                else
                        throw parsing_error("Unrecognized formation string encountered.");

                // Parse the movement type.
                string mt_desc = ps.list[3].atom;
                movement_type mt;

                if(mt_desc == "vertical")
                        mt = movement_type::vertical;

                else if(mt_desc == "horizontal")
                        mt = movement_type::horizontal;

                else if(mt_desc == "diagonal")
                        mt = movement_type::diagonal;

                else if(mt_desc == "zorro")
                        mt = movement_type::zorro;

                else
                        throw parsing_error("unrecognized movement type "
                                        "descriptor encountered.");
                
                // Register pattern.
                patterns.emplace_back(delay, pattern { formation, mt });
        }

        return wave(patterns);
}

static vector<wave> read_waves_from_script(script::dom_node const& root) {

        if(!is_list(root))
                throw parsing_error("Main waves config node isn't a list.");

        vector<wave> waves;
        for(script::dom_node const& wd : root.list)
                waves.push_back(read_wave_from_script(wd));

        return waves;
}

class game_state : public state {

        // External dependencies.
        // ----------------------
        res::resman const& _resman;
        script::scriptman const& _sman;
        ALLEGRO_SAMPLE_ID _music_sid;

        // State.
        // ------
        bool _debug;
        bool _done;
        uint64_t _player_id;
        comm::msg_queue _messages;

        // Systems.
        // --------
        sys::movement_system    _movement_system;
        sys::collision_system   _collision_system;
        sys::arms_system        _arms_system;
        sys::pain_system        _pain_system;
        sys::wellness_system    _wellness_system;
        sys::fx_system          _fx_system;
        sys::drawing_system     _drawing_system;
        sys::score_system       _score_system;
        sys::pickup_system      _pickup_system;
        sys::input_system       _input_system;
        sys::hud_system         _hud_system;
        sys::sound_system       _sound_system;

        // Factories.
        // ----------
        entity_factory _ef;

        // Generation processes.
        // ---------------------
        random_clock<uniform_real_distribution<double>> _star_spawn_clk;
        enemy_manager _en_man;

        // Message handling.
        // -----------------

        void handle_messages(double dt) {

                _messages.visit(dt, [this](comm::message const& msg) {

                        uint64_t id;
                        switch(msg.type) {
                        case comm::msg_t::remove_entity:
                                id = msg.remove_entity.id;
                                remove_node(_movement_system, id);
                                remove_node(_collision_system, id);
                                remove_node(_arms_system, id);
                                remove_node(_pain_system, id);
                                remove_node(_wellness_system, id);
                                remove_node(_fx_system, id);
                                remove_node(_drawing_system, id);
                                remove_node(_score_system, id);
                                remove_node(_pickup_system, id);
                                remove_node(_input_system, id);
                                remove_node(_hud_system, id);
                                remove_node(_sound_system, id);
                                break;

                        case comm::msg_t::spawn_bullet:
                                _ef.create_bullet(
                                        msg.spawn_bullet.x,
                                        msg.spawn_bullet.y,
                                        msg.spawn_bullet.dir_x,
                                        msg.spawn_bullet.dir_y,
                                        msg.spawn_bullet.lin_vel,
                                        msg.spawn_bullet.upgrade_lvl,
                                        msg.spawn_bullet.enemy,
                                        msg.spawn_bullet.origin_id);
                                break;

                        case comm::msg_t::spawn_missile:
                                _ef.create_missile(
                                        msg.spawn_missile.x,
                                        msg.spawn_missile.y,
                                        msg.spawn_missile.dir_x,
                                        msg.spawn_missile.dir_y,
                                        msg.spawn_missile.lin_vel,
                                        msg.spawn_missile.upgrade_lvl,
                                        msg.spawn_missile.enemy,
                                        msg.spawn_missile.origin_id);
                                break;

                        case comm::msg_t::spawn_explosion:
                                _ef.create_explosion(
                                        msg.spawn_explosion.x,
                                        msg.spawn_explosion.y);
                                break;

                        case comm::msg_t::spawn_smoke:
                                _ef.create_smoke(
                                        msg.spawn_smoke.x,
                                        msg.spawn_smoke.y,
                                        msg.spawn_smoke.size);
                                break;

                        case comm::msg_t::spawn_debris:
                                _ef.create_debris(
                                        msg.spawn_debris.x,
                                        msg.spawn_debris.y,
                                        msg.spawn_debris.vx,
                                        msg.spawn_debris.vy,
                                        msg.spawn_debris.vmin,
                                        msg.spawn_debris.vmax,
                                        msg.spawn_debris.theta_min,
                                        msg.spawn_debris.theta_max,
                                        msg.spawn_debris.image,
                                        msg.spawn_debris.explode,
                                        msg.spawn_debris.origin_id);
                                break;

                        case comm::msg_t::spawn_health_pickup:
                                _ef.create_health_pickup(
                                        msg.spawn_health_pickup.x,
                                        msg.spawn_health_pickup.y,
                                        msg.spawn_health_pickup.vx,
                                        msg.spawn_health_pickup.vy);
                                break;

                        case comm::msg_t::spawn_battery_pickup:
                                _ef.create_battery_pickup(
                                        msg.spawn_battery_pickup.x,
                                        msg.spawn_battery_pickup.y,
                                        msg.spawn_battery_pickup.vx,
                                        msg.spawn_battery_pickup.vy);
                                break;
                                
                        case comm::msg_t::spawn_bullet_upgrade_pickup:
                                _ef.create_bullet_upgrade_pickup(
                                        msg.spawn_bullet_upgrade_pickup.x,
                                        msg.spawn_bullet_upgrade_pickup.y,
                                        msg.spawn_bullet_upgrade_pickup.vx,
                                        msg.spawn_bullet_upgrade_pickup.vy);
                                break;

                        case comm::msg_t::spawn_missile_upgrade_pickup:
                                _ef.create_missile_upgrade_pickup(
                                        msg.spawn_missile_upgrade_pickup.x,
                                        msg.spawn_missile_upgrade_pickup.y,
                                        msg.spawn_missile_upgrade_pickup.vx,
                                        msg.spawn_missile_upgrade_pickup.vy);
                                break;

                        default:
                                cerr << "Unrecognized message type found." << endl;
                                exit(1);
                                break;
                        }
                });
        }

public:
        game_state(const res::resman& resman, script::scriptman const& sman)
        : _resman(resman)
        , _sman(sman)
        , _debug(false)
        , _done(false)
        , _drawing_system(resman.get_font(res::res_id::TINY_FONT))
        , _score_system(map<cmp::score_class, double> {
                { cmp::score_class::PLAYER, cfg::real("gameplay_score_for_player") },
                { cmp::score_class::ENEMY_LIGHT_FIGHTER, cfg::real("gameplay_score_for_lfighter") },
                { cmp::score_class::ENEMY_HEAVY_FIGHTER, cfg::real("gameplay_score_for_hfighter") },
                { cmp::score_class::ENEMY_LIGHT_BOMBER, cfg::real("gameplay_score_for_lbomber")},
                { cmp::score_class::ENEMY_HEAVY_BOMBER, cfg::real("gameplay_score_for_hbomber") } })
        , _hud_system(
                _resman.get_bitmap(res::res_id::HUD_BG),
                _resman.get_bitmap(res::res_id::HEALTH),
                _resman.get_bitmap(res::res_id::BATTERY),
                _resman.get_bitmap(res::res_id::DIODE_ON),
                _resman.get_bitmap(res::res_id::DIODE_OFF),
                _resman.get_bitmap(res::res_id::B_UPGRADE),
                _resman.get_bitmap(res::res_id::M_UPGRADE),
                _resman.get_font(res::res_id::FONT),
                _resman.get_font(res::res_id::TINY_FONT),
                cfg::integer("gfx_screen_w"),
                cfg::integer("gfx_screen_h"))
        , _sound_system(_resman)
        , _ef(_resman,
                _movement_system,
                _collision_system,
                _arms_system,
                _pain_system,
                _wellness_system,
                _fx_system,
                _drawing_system,
                _score_system,
                _pickup_system,
                _input_system,
                _hud_system,
                _sound_system)
        , _star_spawn_clk(
                        uniform_real_distribution<double>(
                                cfg::real("gfx_star_interval_min"),
                                cfg::real("gfx_star_interval_max")),
                        bind(&entity_factory::create_star, &_ef))
        , _en_man(read_waves_from_script(_sman.get_dom("waves")))
        {
                // Spawn initial stars.
                uniform_real_distribution<double> x_dist(1.0, cfg::integer("gfx_screen_w") - 1);
                uniform_real_distribution<double> y_dist(1.0, cfg::integer("gfx_screen_h") - 1);
                for(unsigned i = 0; i < (unsigned)cfg::integer("gfx_star_initial_count"); ++i) {
                        const double x = x_dist(rnd::engine);
                        const double y = x_dist(rnd::engine);
                        _ef.create_star_xy(x, y);
                }
                _player_id = _ef.create_player_ship(
                                cfg::real("gameplay_player_start_x"),
                                cfg::real("gameplay_player_start_y"));

                // Begin playing music.
                ALLEGRO_SAMPLE* sample = _resman.get_sample(res::res_id::INGAME_MUSIC);
                al_play_sample(sample,
                        0.5, 0, 1,
                        ALLEGRO_PLAYMODE_LOOP,
                        &_music_sid);
        }

        ~game_state() {
                al_stop_sample(&_music_sid);
        }

        void sigkill() {
                _done = true;
        }

        bool done() {
                return _done;
        }

        unique_ptr<state> next_state() {
                return create_menu_state(_resman, _sman);
        }

        void update(double t, double dt) {
                
                // Trigger the clocks.
                _star_spawn_clk.tick(dt);

                // Manage the gameplay rules.
                bool keep_going = _en_man.tick(
                        dt, _ef,
                        cfg::integer("gfx_screen_w"),
                        cfg::integer("gfx_screen_h"));

                if(!keep_going) {
                        _en_man.reset();
                }

                // Manage the debug ouptut. 
                _movement_system.set_debug_mode(_debug);
                _collision_system.set_debug_mode(_debug);
                _arms_system.set_debug_mode(_debug);
                _pain_system.set_debug_mode(_debug);
                _wellness_system.set_debug_mode(_debug);
                _fx_system.set_debug_mode(_debug);
                _drawing_system.set_debug_mode(_debug);
                _score_system.set_debug_mode(_debug);
                _pickup_system.set_debug_mode(_debug);
                _input_system.set_debug_mode(_debug);
                _hud_system.set_debug_mode(_debug);
                _sound_system.set_debug_mode(_debug);

                // Update logic systems.
                _movement_system.update(dt, _messages);
                _collision_system.update();
                _arms_system.update(dt, _messages);
                _pain_system.update(_messages);
                _wellness_system.update(dt, _messages);
                _fx_system.update(dt, _messages);
                _score_system.update();
                _pickup_system.update(_messages); 
                _input_system.update();
                _sound_system.update(dt);

                // Handle messages.
                handle_messages(dt);
        }

        void draw(double weight) {
                _drawing_system.update(weight);
                _hud_system.update();
        }

        void key_up(int k) {
                if(k == ALLEGRO_KEY_ESCAPE) _done = true;
                if(k == ALLEGRO_KEY_SPACE) _debug = false;
                _input_system.key_up(k);
        }

        void key_down(int k) {
                if(k == ALLEGRO_KEY_SPACE) _debug = true;
                _input_system.key_down(k);
        }
};

unique_ptr<state> create_game_state(
                res::resman const& res,
                script::scriptman const& sman) {
        return unique_ptr<state>(new game_state(res, sman));
}
