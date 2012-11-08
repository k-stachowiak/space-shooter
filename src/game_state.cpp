#include <functional>
#include <algorithm>
#include <sstream>
#include <random>
#include <memory>
#include <vector>
#include <limits>
#include <map>
#include <iostream>
using namespace std;
using namespace std::placeholders;

#include <allegro5/allegro_primitives.h>

#include "config.h"
#include "game_model.h"
#include "state.h"
#include "random_clock.h"
#include "collision/collision_space.h"
#include "entities/entity_factory.h"

class game_state : public state {			

	default_random_engine _engine;

	const config& _config;
	const resman& _resman;

	bool _done;
	map<int, bool> _keymap;

	game_model _game_model;
	entity_factory _entity_factory;
	collision_space _coll_space;
	vector<entity> _entities;

	ALLEGRO_FONT* _font;

	random_clock<exponential_distribution<double>> _star_spawn_clk;
	random_clock<uniform_real_distribution<double>> _eye_spawn_clk;

	// Initial entities setup.
	// -----------------------
	void initialize_entities() {

		// Initialize the player's ship.
		_entities.push_back(
			_entity_factory.create_player_ship(
			_config.get_screen_w() * 0.5,
			_config.get_screen_h() - 100.0, 
			_config.get_screen_w(),
			_config.get_screen_h(),
			-3.1415 * 0.5,
			_config.get_shoot_period(),
			_config.get_player_max_health(),
			_config.get_player_max_armor(),
			_coll_space));

		_game_model.player_health = _config.get_player_max_health();

		// Initialize stars.
		static const uint32_t INIT_STARS = 33;

		uniform_real_distribution<double> x_dist(0.0, _config.get_screen_w());
		uniform_real_distribution<double> y_dist(0.0, _config.get_screen_h());
		uniform_real_distribution<double> z_dist(0.0, 1.0);

		for(uint32_t i = 0; i < INIT_STARS; ++i) {

			double x = x_dist(_engine);
			double y = y_dist(_engine);
			double z = z_dist(_engine);
			double near_v = _config.get_near_star_v();

			_entities.push_back(_entity_factory.create_star(
				x, y, near_v, z, 
				_config.get_screen_w(), 
				_config.get_screen_h()));
		}
	}

	// Prepare state for frame.
	// ------------------------
	void init_frame_state() {

		// Player controls.
		double steer_x = 0.0;
		double steer_y = 0.0;
		if(_keymap[ALLEGRO_KEY_UP]) { steer_y -= 1.0; }
		if(_keymap[ALLEGRO_KEY_DOWN]) { steer_y += 1.0; }
		if(_keymap[ALLEGRO_KEY_LEFT]) { steer_x -= 1.0; }
		if(_keymap[ALLEGRO_KEY_RIGHT]) { steer_x += 1.0; }
		_game_model.steering_vec[0] = steer_x;
		_game_model.steering_vec[1] = steer_y;	

		// Player weapons.
		_game_model.trigger_down = _keymap[ALLEGRO_KEY_LCTRL];	
	}

	// Entity spawning routines.
	// -------------------------

	void spawn_star() {
		uniform_real_distribution<double> x_dist(0.0, _config.get_screen_w());
		uniform_real_distribution<double> z_dist(0.0, 1.0);

		double y = 1.0;
		double x = x_dist(_engine);
		double z = z_dist(_engine);
		double near_v = _config.get_near_star_v();

		_entities.push_back(_entity_factory.create_star(
			x, y, near_v, z, 
			_config.get_screen_w(), 
			_config.get_screen_h()));
	}

	void spawn_eye_enemy() {
		_entities.push_back(_entity_factory.create_enemy_eye(
			_config.get_screen_w(), _config.get_screen_h(),
			_coll_space));
	}

	void spawn_bullet(	double x, double y,
				double vx, double vy,
				res_id bitmap_id,
				bool is_enemy) {
		_entities.push_back(
			_entity_factory.create_bullet(
				x, y, vx, vy, 
				_config.get_screen_w(), 
				_config.get_screen_h(), 
				bitmap_id,
				_coll_space,
				is_enemy
					? collision_class::ENEMY_BULLET
					: collision_class::PLAYER_BULLET));
	}

	void spawn_explosion(uint32_t num_debris, double x, double y, double vx, double vy) {
		_entities.push_back(_entity_factory.create_explosion(x, y, vx, vy));
		for(uint32_t i = 0; i < num_debris; ++i)
			_entities.push_back(_entity_factory.create_debris(x, y, vx, vy));
	}

	void spawn_smoke(double x, double y) {
		_entities.push_back(_entity_factory.create_smoke(x, y));
	}

	void spawn_health(double x, double y, double vx, double vy) {
		_entities.push_back(_entity_factory.create_health_pickup(
					x, y,
					vx, vy,
					_config.get_screen_w(),
					_config.get_screen_h(),
					_coll_space));
	}

	// Game messages system.
	// ---------------------

	void remove_entity(uint32_t id) {
		for(uint32_t i = 0; i < _entities.size(); ++i) {
			if(_entities[i].get_id() == id) {
				_entities[i].deinitialize_components(_game_model);
				_entities[i] = _entities.back();
				_entities.pop_back();
				return;
			}
		}
	}

	void handle_messages() {
		while(!_game_model.game_msgs.empty()) {

			const game_msg& msg = _game_model.game_msgs.back();

			switch(msg.type) {

			case game_msg_t::REMOVE_ENTITY:
				remove_entity(msg.remove_entity.id);		
				break;

			case game_msg_t::SPAWN_BULLET:
				spawn_bullet(
					msg.spawn_bullet.x,
					msg.spawn_bullet.y,
					msg.spawn_bullet.vx,
					msg.spawn_bullet.vy,
					msg.spawn_bullet.bitmap_id,
					msg.spawn_bullet.is_enemy);
				break;

			case game_msg_t::SPAWN_EXPL:
				spawn_explosion(
					msg.spawn_expl.num_debris,
					msg.spawn_expl.x,
					msg.spawn_expl.y,
					msg.spawn_expl.vx,
					msg.spawn_expl.vy);
				break;

			case game_msg_t::SPAWN_SMOKE:
				spawn_smoke(
					msg.spawn_smoke.x,
					msg.spawn_smoke.y);
				break;

			case game_msg_t::SPAWN_HEALTH:
				spawn_health(
					msg.spawn_health.x,
					msg.spawn_health.y,
					msg.spawn_health.vx,
					msg.spawn_health.vy);
				break;

			default:
				break;
			}

			_game_model.game_msgs.pop_back();
		}
	}

	void log_entites_states() {
		// Print the entites count.
		cout << "Entities: " << _entities.size() << endl;

		// Print the collision space entries count.
		cout << "Coll. entries: " << _coll_space.num_entries() << endl;

		// Print component counts.
		for(auto const& e : _entities) {
			uint32_t cmps = 0;
			e.visit_components([&cmps](component const&) {
				++cmps;
			});
			cout << cmps << '\t';
		}
		cout << endl;
	}

	// Rendering.
	// ----------
	
	ALLEGRO_COLOR health_color(double health_ratio) {

		double r, g;

		if(health_ratio >= 0.5) {
			g = 1.0;
			r = 1.0 - (health_ratio - 0.5) * 2.0;
		} else {
			r = 1.0;
			g = health_ratio * 2.0;
		}

		return al_map_rgb_f(r, g, 0.0);
	}

	void draw_bar(double from_bottom, double ratio, ALLEGRO_COLOR color) {
		double x1 = _config.get_screen_w() * 0.05;
		double x2 = _config.get_screen_w() * 0.95;
		double y1 = _config.get_screen_h() - from_bottom - 5.0f;
		double y2 = _config.get_screen_h() - from_bottom;
		double hx2 = x1 + ratio * (x2 - x1);
		al_draw_filled_rectangle(x1, y1, hx2, y2, color);
	}

	void print_hud() {

		// Score part.
		// -----------
		al_draw_textf(_font,
			al_map_rgba_f(1, 1, 1, 1), 10, 10, 0,
			"score %d", (int)_game_model.score);

		// Health part.
		// ------------
		double health_ratio =
			_game_model.player_health /
			_config.get_player_max_health();

		draw_bar(10.0, health_ratio, health_color(health_ratio));

		double armor_ratio =
			_game_model.player_armor /
			_config.get_player_max_armor();

		draw_bar(20.0, armor_ratio, al_map_rgb_f(0.333f, 0.444f, 0.667f));
	}

public:
	game_state(const config& config, const resman& resman)
	: _config(config)
	, _resman(resman)
	, _done(false)
	, _entity_factory(_config, _resman)
	, _font(_resman.get_font(res_id::FONT))
	, _star_spawn_clk(
		exponential_distribution<double>(5.0),
		bind(&game_state::spawn_star, this))
	, _eye_spawn_clk(
		uniform_real_distribution<double>(1.0, 2.0),
		bind(&game_state::spawn_eye_enemy, this)) {

		// Initialize the keymap.
		_keymap[ALLEGRO_KEY_UP] = false;
		_keymap[ALLEGRO_KEY_DOWN] = false;
		_keymap[ALLEGRO_KEY_LEFT] = false;
		_keymap[ALLEGRO_KEY_RIGHT] = false;
		_keymap[ALLEGRO_KEY_LCTRL] = false;
		_keymap[ALLEGRO_KEY_SPACE] = false;	

		initialize_entities();
	}

	~game_state() {
		for(entity& e : _entities)
			e.deinitialize_components(_game_model);
	}

	void sigkill() {
		_done = true;
	}

	bool done() {
		return _done;
	}

	unique_ptr<state> next_state() {
		return unique_ptr<state>();
	}

	void frame_logic(double dt) {

		_game_model.debug = _keymap[ALLEGRO_KEY_SPACE];

		// Frame setup.
		// --------------------
		al_clear_to_color(al_map_rgb_f(0.0f, 0.0f, 0.0f));
		_game_model.last_dt = dt;
		init_frame_state();

		// Do frame.
		// ---------
		for(auto& e : _entities)
			e.action(_config, _game_model);

		print_hud();
		_coll_space.check_collisions();
		_eye_spawn_clk.tick(dt);
		_star_spawn_clk.tick(dt);

		// Handle global requests.
		// -----------------------
		handle_messages();

		// Log this and that.
		// ------------------
		log_entites_states();
	}

	void key_up(int k) {

		if(k == ALLEGRO_KEY_ESCAPE) {
			_done = true;
			return;
		}

		_keymap[k] = false;
	}

	void key_down(int k) {
		_keymap[k] = true;
	}
};

unique_ptr<state> create_game_state(const config& config, const resman& res) {
	return unique_ptr<state>(new game_state(config, res));
}
