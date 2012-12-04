#include <functional>
using std::bind;

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

#include "state.h"
#include "../misc/random_clock.h"
#include "../misc/rand.h"
#include "../geometry/bezier.h"
#include "../cmp/systems.h"
#include "../cmp/comm.h"

// TODO:
// - make smoke scalable: small smoke for rockets, big for bad health
// - HUD

class test_state : public state {

	// External dependencies.
	// ----------------------
	const config& _config;
	const resman& _resman;

	// State.
	// ------
	map<int, bool> _keys;
	bool _done;
	vector<comm::message> _messages;

	// Generation processes.
	// ---------------------
	random_clock<uniform_real_distribution<double>> _eye_spawn_clk;
	random_clock<uniform_real_distribution<double>> _bomber_spawn_clk;

	// Systems.
	// --------
	sys::movement_system _ms;
	sys::collision_system _cs;
	sys::arms_system _as;
	sys::drawing_system _ds;
	sys::pain_system _ps;
	sys::wellness_system _ws;
	sys::fx_system _fs;

	// Factories.
	// ----------
	uint64_t _last_id;

	uint64_t create_explosion(double x, double y) {

		// Initialize components.
		uint64_t id = ++_last_id;
		
		uint32_t frame_width = 70;
		uint32_t num_frames = 16;
		double frame_time = 0.05;
		vector<cmp::frame_def> frame_defs;
		for(uint32_t i = 0; i < num_frames; ++i)
			frame_defs.emplace_back(i, frame_time);

		auto appearance = cmp::create_simple_anim(
				_resman.get_bitmap(res_id::EXPLOSION_1),
				frame_width,
				num_frames,
				frame_defs,
				1);

		vector<shared_ptr<cmp::dynamics>> dynamics;

		auto orientation = cmp::create_orientation(x, y, 0.0);

		shared_ptr<cmp::shape> shape;

		shared_ptr<cmp::wellness> wellness;

		auto ttl = cmp::create_const_int_timer(num_frames * frame_time);

		bool explodes = false;

		uint32_t num_debris = 0;

		// Register nodes.
		_ds.add_node({ id, appearance, orientation, shape, dynamics });
		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		// Feedback for the state.
		return id;
	}

	uint64_t create_smoke(double x, double y) {

		// Initialize components.
		uint64_t id = ++_last_id;
		
		uint32_t frame_width = 32 * 3;
		uint32_t num_frames = 25;
		double frame_time = 0.075;
		vector<cmp::frame_def> frame_defs;
		for(uint32_t i = 0; i < num_frames; ++i)
			frame_defs.emplace_back(i, frame_time);

		auto appearance = cmp::create_simple_anim(
				_resman.get_bitmap(res_id::SMOKE_1),
				frame_width,
				num_frames,
				frame_defs,
				1);

		vector<shared_ptr<cmp::dynamics>> dynamics;

		auto orientation = cmp::create_orientation(x, y, 0.0);

		shared_ptr<cmp::shape> shape;

		shared_ptr<cmp::wellness> wellness;

		auto ttl = cmp::create_const_int_timer(num_frames * frame_time);

		bool explodes = false;

		uint32_t num_debris = 0;

		// Register nodes.
		_ds.add_node({ id, appearance, orientation, shape, dynamics });
		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		// Feedback for the state.
		return id;
	}

	uint64_t create_debris(double x, double y, double vx, double vy) {

		// Prepare helpers.
		vector<res_id> bitmaps {
			res_id::DEBRIS1,
			res_id::DEBRIS2,
			res_id::DEBRIS3,
			res_id::DEBRIS4,
			res_id::DEBRIS5 };

		uniform_int_distribution<int> bmp_dist(0, bitmaps.size() - 1);
		uint32_t index = bmp_dist(rnd::engine);
		res_id bmp = bitmaps[index];

		uniform_real_distribution<double> ttl_dist(1.0, 3.0);
		double ttl_time = ttl_dist(rnd::engine);

		// Initialize components.
		uint64_t id = ++_last_id;
		
		auto appearance = cmp::create_static_bmp(_resman.get_bitmap(bmp));

		vector<shared_ptr<cmp::dynamics>> dynamics {
			cmp::create_const_velocity_dynamics(vx, vy),
			cmp::create_const_ang_vel_dynamics(1.5)
		};

		auto orientation = cmp::create_orientation(x, y, 0.0);

		shared_ptr<cmp::shape> shape;

		shared_ptr<cmp::wellness> wellness;
		
		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto ttl = cmp::create_const_int_timer(ttl_time);

		bool explodes = false;

		uint32_t num_debris = 0;

		// Register nodes.
		_ds.add_node({ id, appearance, orientation, shape, dynamics });
		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });
		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds });

		// Feedback for the state.
		return id;
	}

	uint64_t create_player_ship(double x, double y) {

		// Initialize components.
		uint64_t id = ++_last_id;

		auto appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::PLAYER_SHIP));

		vector<shared_ptr<cmp::dynamics>> dynamics;

		auto orientation = cmp::create_orientation(x, y, -1.57);

		auto movement_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto life_bounds = shared_ptr<cmp::bounds>();

		auto cc = cmp::coll_class::PLAYER_SHIP;

		auto shape = cmp::create_circle(x, y, 32.0);

		auto coll_queue = cmp::create_coll_queue();

		vector<shared_ptr<cmp::weapon_beh>> weapon_beh;

		auto painmap = cmp::create_painmap({
			{ cmp::coll_class::ENEMY_BULLET, 10.0 },
			{ cmp::coll_class::ENEMY_SHIP, 25.0 }
		});

		auto wellness = cmp::create_wellness(100.0);

		shared_ptr<cmp::timer> ttl;

		vector<shared_ptr<cmp::fx>> fxs {
			cmp::create_smoke_when_hurt(0.5)
		};

		bool explodes = true;

		uint32_t num_debris = 10;

		// Register nodes.
		_ds.add_node({ id, appearance, orientation, shape, dynamics });

		_ms.set_player_controlled(id);
		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});

		_as.add_node({id, orientation, weapon_beh});
		_as.set_player_shooting(id);
		_as.set_player_interval(0.125);

		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		_fs.add_node({ id, orientation, wellness, fxs });

		return id;
	}

	uint64_t create_bomber() {

		uniform_real_distribution<double> x_dist(
				0.0, _config.get_screen_w());

		double x = x_dist(rnd::engine);
		double y = 1.0;

		// Initialize components.
		
		uint64_t id = ++_last_id;

		auto appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::ENEMY_BOMBER));

		vector<shared_ptr<cmp::dynamics>> dynamics {
			cmp::create_const_velocity_dynamics(0.0, 60.0)
		};

		auto orientation = cmp::create_orientation(x, y, 1.57);

		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto cc = cmp::coll_class::ENEMY_SHIP;

		auto shape = cmp::create_circle(x, y, 32.0);

		auto coll_queue = cmp::create_coll_queue();

		vector<shared_ptr<cmp::weapon_beh>> weapon_beh {
			cmp::create_period_missile(3.0, 3.0, -15.0, 0.0),
			cmp::create_period_missile(3.0, 3.0,  15.0, 0.0)
		};

		auto painmap = cmp::create_painmap({
				{ cmp::coll_class::PLAYER_BULLET, 10.0 },
				{ cmp::coll_class::PLAYER_SHIP, 50.0 } });

		auto wellness = cmp::create_wellness(100.0);

		shared_ptr<cmp::timer> ttl;

		vector<shared_ptr<cmp::fx>> fxs {
			cmp::create_smoke_when_hurt(0.5)
		};

		bool explodes = true;

		uint32_t num_debris = 7;

		// Register the components.
		// ------------------------
		_ds.add_node({ id, appearance, orientation, shape, dynamics });

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});

		_as.add_node({ id, orientation, weapon_beh });

		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		_fs.add_node({ id, orientation, wellness, fxs });

		return id;
	}

	uint64_t create_eye() {
		
		// Prepare resources for the components.
		// -------------------------------------

		// The animation.
		uint32_t frame_width = 32;
		uint32_t num_frames = 6;
		double blink_time = 0.15;
		double angry_time = 0.15;

		vector<cmp::frame_def> frame_defs;	
		frame_defs.emplace_back(4, angry_time);
		frame_defs.emplace_back(3, angry_time);
		frame_defs.emplace_back(2, blink_time); // Close.
		frame_defs.emplace_back(1, blink_time);
		frame_defs.emplace_back(0, blink_time); // Open.
		frame_defs.emplace_back(1, blink_time);
		frame_defs.emplace_back(2, blink_time); // Close.
		frame_defs.emplace_back(1, blink_time);
		frame_defs.emplace_back(0, blink_time); // Open.
		frame_defs.emplace_back(1, blink_time);
		frame_defs.emplace_back(2, blink_time);
		frame_defs.emplace_back(3, angry_time); // Angry
		frame_defs.emplace_back(4, angry_time);
		frame_defs.emplace_back(5, angry_time);

		double anim_period = 0;
		for(auto& fd : frame_defs)
			anim_period += fd.time;

		// The path.
		uniform_real_distribution<double> from_to_dist(
				0.0, _config.get_screen_w());

		uniform_real_distribution<double> pan_dist(
				0.0, _config.get_screen_w());

		bernoulli_distribution right_to_left(0.5);

		vector<point> points;
		points.emplace_back(from_to_dist(rnd::engine), 0.0);
		points.emplace_back(0.0, pan_dist(rnd::engine));
		points.emplace_back(_config.get_screen_w(), pan_dist(rnd::engine));
		points.emplace_back(
				from_to_dist(rnd::engine), _config.get_screen_h() + 50);
		if(right_to_left(rnd::engine)) swap(points[2], points[1]);
		
		points = bezier(points, 50);

		// Prepare the components.
		// -----------------------
		uint64_t id = ++_last_id;

		auto appearance = cmp::create_simple_anim(
				_resman.get_bitmap(res_id::ENEMY_EYE),
				frame_width,
				num_frames,
				frame_defs,
				-1);

		vector<shared_ptr<cmp::dynamics>> dynamics {
			cmp::create_path_dynamics(points)
		};

		auto orientation = cmp::create_orientation(
				points.front().x,
				points.front().y,
				0.0);

		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto cc = cmp::coll_class::ENEMY_SHIP;

		auto shape = cmp::create_circle(
				points.front().x,
				points.front().y,
				16.0);

		auto coll_queue = cmp::create_coll_queue();

		vector<shared_ptr<cmp::weapon_beh>> weapon_beh {
			cmp::create_period_bullet(anim_period, anim_period)
		};

		auto painmap = cmp::create_painmap({
				{ cmp::coll_class::PLAYER_BULLET, 10.0 },
				{ cmp::coll_class::PLAYER_SHIP, 50.0 } });

		auto wellness = cmp::create_wellness(30.0);

		shared_ptr<cmp::timer> ttl;

		vector<shared_ptr<cmp::fx>> fxs {
			cmp::create_smoke_when_hurt(0.5)
		};

		bool explodes = true;

		uint32_t num_debris = 5;

		// Register the components.
		// ------------------------
		_ds.add_node({ id, appearance, orientation, shape, dynamics });

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});

		_as.add_node({ id, orientation, weapon_beh });

		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		_fs.add_node({ id, orientation, wellness, fxs });

		return id;
	}

	uint64_t create_missile(
			double x, double y,
			double theta,
			double vx, double vy,
			bool enemy) {

		// Constants.
		// ----------
		double missile_health = 1.0;

		// Initialize Components.
		// ----------------------

		// Common components.

		uint64_t id = ++_last_id;

		auto appearance = cmp::create_static_bmp( _resman.get_bitmap(res_id::MISSILE));

		vector<shared_ptr<cmp::dynamics>> dynamics {
			cmp::create_const_velocity_dynamics(vx, vy)
		};

		auto orientation = cmp::create_orientation(x, y, theta);

		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto shape = cmp::create_circle(x, y, 8.0);

		auto coll_queue = cmp::create_coll_queue();

		auto wellness = cmp::create_wellness(missile_health);

		shared_ptr<cmp::timer> ttl;

		vector<shared_ptr<cmp::fx>> fxs {
			cmp::create_period_smoke(0.25, 0.25)
		};

		bool explodes = true;

		uint32_t num_debris = 3;

		// Context dependent.

		shared_ptr<cmp::painmap> painmap;
		cmp::coll_class cc;
		if(enemy) {
			cc = cmp::coll_class::ENEMY_MISSILE;
			painmap = cmp::create_painmap({
					{ cmp::coll_class::PLAYER_SHIP, missile_health },
					{ cmp::coll_class::PLAYER_BULLET, missile_health } });
		} else {
			cc = cmp::coll_class::PLAYER_MISSILE;
			painmap = cmp::create_painmap({
					{ cmp::coll_class::ENEMY_SHIP, missile_health },
					{ cmp::coll_class::ENEMY_BULLET, missile_health } });
		}

		// Register nodes.
		// ---------------

		_ds.add_node({ id, appearance, orientation, shape, dynamics });

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});
		
		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		_fs.add_node({ id, orientation, wellness, fxs });

		return id;
	}

	uint64_t create_bullet(
			double x, double y,
			double theta,
			double vx, double vy,
			bool enemy) {

		// Constants.
		// ----------
		double bullet_health = 1.0;

		// Initialize components.
		// ----------------------

		// Common components.

		uint64_t id = ++_last_id;

		vector<shared_ptr<cmp::dynamics>> dynamics {
			cmp::create_const_velocity_dynamics(vx, vy)
		};

		auto orientation = cmp::create_orientation(x, y, theta);

		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto shape = cmp::create_circle(x, y, 8.0);

		auto coll_queue = cmp::create_coll_queue();

		auto wellness = cmp::create_wellness(bullet_health);

		shared_ptr<cmp::timer> ttl;

		bool explodes = false;

		uint32_t num_debris = 0;

		// Context dependent.

		shared_ptr<cmp::appearance> appearance;
		shared_ptr<cmp::painmap> painmap;
		cmp::coll_class cc;
		if(enemy) {
			appearance = cmp::create_static_bmp( _resman.get_bitmap(res_id::EYE_BULLET));
			painmap = cmp::create_painmap({ { cmp::coll_class::PLAYER_SHIP, bullet_health } });
			cc = cmp::coll_class::ENEMY_BULLET;
		} else {
			appearance = cmp::create_static_bmp( _resman.get_bitmap(res_id::PLAYER_BULLET));
			painmap = cmp::create_painmap({ { cmp::coll_class::ENEMY_SHIP, bullet_health } });
			cc = cmp::coll_class::PLAYER_BULLET;
		}

		// Register nodes.
		// ---------------

		_ds.add_node({ id, appearance, orientation, shape, dynamics });

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});
		
		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		_ws.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

		return id;
	}

	// Message handling.
	// -----------------

	void handle_message(comm::message const& msg) {
		uint64_t id;
		switch(msg.type) {
		case comm::msg_t::remove_entity:
			id = msg.remove_entity.id;
			sys::remove_node(_ms, id);
			sys::remove_node(_cs, id);
			sys::remove_node(_as, id);
			sys::remove_node(_ds, id);
			sys::remove_node(_ps, id);
			sys::remove_node(_ws, id);
			sys::remove_node(_fs, id);
			break;

		case comm::msg_t::spawn_bullet:
			create_bullet(msg.spawn_bullet.x,
					msg.spawn_bullet.y,
					msg.spawn_bullet.theta,
					msg.spawn_bullet.vx,
					msg.spawn_bullet.vy,
					msg.spawn_bullet.enemy);
			break;

		case comm::msg_t::spawn_missile:
			create_missile(msg.spawn_missile.x,
					msg.spawn_missile.y,
					msg.spawn_missile.theta,
					msg.spawn_missile.vx,
					msg.spawn_missile.vy,
					msg.spawn_missile.enemy);
			break;

		case comm::msg_t::spawn_explosion:
			create_explosion(msg.spawn_explosion.x,
					msg.spawn_explosion.y);
			break;

		case comm::msg_t::spawn_smoke:
			create_smoke(msg.spawn_smoke.x, msg.spawn_smoke.y);
			break;

		case comm::msg_t::spawn_debris:
			create_debris(msg.spawn_debris.x,
					msg.spawn_debris.y,
					msg.spawn_debris.vx,
					msg.spawn_debris.vy);
			break;
			
		default:
			break;
		}
	}

public:
	test_state(const config& config, const resman& resman)
	: _config(config)
	, _resman(resman)
	, _done(false)
	, _eye_spawn_clk(
		uniform_real_distribution<double>(1.0, 2.0),
		bind(&test_state::create_eye, this))
	, _bomber_spawn_clk(
		uniform_real_distribution<double>(5.0, 7.0),
		bind(&test_state::create_bomber, this))
	, _ds(resman.get_font(res_id::TINY_FONT))
	, _last_id(0)
	{
		_keys[ALLEGRO_KEY_UP] = false;
		_keys[ALLEGRO_KEY_DOWN] = false;
		_keys[ALLEGRO_KEY_LEFT] = false;
		_keys[ALLEGRO_KEY_RIGHT] = false;
		_keys[ALLEGRO_KEY_LCTRL] = false;
		create_player_ship(200.0, 200.0);
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

		// Trigger the clocks.
		_eye_spawn_clk.tick(dt);
		_bomber_spawn_clk.tick(dt);

		// Update the player's throttle.
		double player_throttle_x = 0.0;
	       	double player_throttle_y = 0.0;
		if(_keys[ALLEGRO_KEY_RIGHT]) player_throttle_x += 1.0;
		if(_keys[ALLEGRO_KEY_LEFT]) player_throttle_x -= 1.0;
		if(_keys[ALLEGRO_KEY_DOWN]) player_throttle_y += 1.0;
		if(_keys[ALLEGRO_KEY_UP]) player_throttle_y -= 1.0;
		_ms.set_player_throttle(player_throttle_x, player_throttle_y);

		// Update the player's trigger.
		bool player_trigger = _keys[ALLEGRO_KEY_LCTRL];
		_as.set_player_trigger(player_trigger);

		// Manage the debug ouptut. 
		_ms.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_cs.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_as.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_ds.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_ps.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_ws.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);
		_fs.set_debug_mode(_keys[ALLEGRO_KEY_SPACE]);

		// Update the systems.
		_ms.update(dt, _messages);
		_cs.update();
		_as.update(dt, _messages);
		_ds.update(dt);
		_ps.update(_messages);
		_ws.update(dt, _messages);
		_fs.update(dt, _messages);

		// Handle messages.
		while(!_messages.empty()) {
			handle_message(_messages.back());
			_messages.pop_back();
		}
	}

	void key_up(int k) {
		if(k == ALLEGRO_KEY_ESCAPE)
			_done = true;
		_keys[k]= false;
	}

	void key_down(int k) {
		_keys[k]= true;
	}
};

unique_ptr<state> create_test_state(const config& config, const resman& res) {
	return unique_ptr<state>(new test_state(config, res));
}
