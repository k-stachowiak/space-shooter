#include <random>
using std::default_random_engine;

#include "state.h"
#include "geometry/bezier.h"
#include "cmp/systems.h"
#include "cmp/comm.h"

class test_state : public state {

	// Basic utilities.
	// ----------------
	default_random_engine _engine;

	// External dependencies.
	// ----------------------
	const config& _config;
	const resman& _resman;

	// State.
	// ------
	map<int, bool> _keys;
	bool _done;
	vector<comm::message> _messages;

	// Systems.
	// --------
	sys::movement_system _ms;
	sys::collision_system _cs;
	sys::arms_system _as;
	sys::drawing_system _ds;
	sys::pain_system _ps;

	// Factories.
	// ----------
	uint64_t _last_id;

	uint64_t create_explosion(double x, double y) {

		// Initialize components.
		uint64_t id = ++_last_id;
		
		uint32_t frame_width = 70;
		uint32_t num_frames = 16;
		vector<cmp::frame_def> frame_defs;
		for(uint32_t i = 0; i < num_frames; ++i)
			frame_defs.emplace_back(i, 0.05);

		auto appearance = cmp::create_simple_anim(
				_resman.get_bitmap(res_id::EXPLOSION_1),
				frame_width,
				num_frames,
				frame_defs,
				1);

		auto orientation = cmp::create_orientation(x, y, 0.0);

		// Register nodes.
		_ds.add_node({id, appearance, orientation});

		// Feedback for the state.
		return id;
	}

	uint64_t create_player_ship(double x, double y) {

		// Initialize components.
		uint64_t id = ++_last_id;

		auto appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::PLAYER_SHIP));

		auto dynamics = shared_ptr<cmp::dynamics>();

		auto orientation = cmp::create_orientation(x, y, -1.57);

		auto movement_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto life_bounds = shared_ptr<cmp::bounds>();

		auto cc = cmp::coll_class::PLAYER_SHIP;

		auto shape = cmp::create_circle(x, y, 32.0);

		auto coll_queue = cmp::create_coll_queue();

		auto painmap = cmp::create_painmap({
				{ cmp::coll_class::ENEMY_BULLET, 10.0 },
				{ cmp::coll_class::ENEMY_SHIP, 25.0 } });

		auto wellness = cmp::create_wellness(100.0);

		// Register nodes.
		_ds.add_node({id, appearance, orientation});

		_ms.set_player_controlled(id);
		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});

		_as.add_node({id, orientation});
		_as.set_player_shooting(id);
		_as.set_player_interval(0.125);

		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

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
		points.emplace_back(from_to_dist(_engine), 0.0);
		points.emplace_back(0.0, pan_dist(_engine));
		points.emplace_back(_config.get_screen_w(), pan_dist(_engine));
		points.emplace_back(
				from_to_dist(_engine), _config.get_screen_h() + 50);
		if(right_to_left(_engine)) swap(points[2], points[1]);
		
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

		auto dynamics = cmp::create_path_dynamics(points);

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

		auto painmap = cmp::create_painmap({
				{ cmp::coll_class::PLAYER_BULLET, 10.0 },
				{ cmp::coll_class::PLAYER_SHIP, 50.0 } });

		auto wellness = cmp::create_wellness(30.0);

		// Register the components.
		// ------------------------
		_ds.add_node({id, appearance, orientation});

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});

		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

		return id;
	}

	uint64_t create_bullet(
			double x, double y,
			double theta,
			double vx, double vy,
			cmp::coll_class cc) {

		// Constants.
		double bullet_health = 1.0;

		// Initialize components.

		uint64_t id = ++_last_id;

		auto appearance = cmp::create_static_bmp(
				_resman.get_bitmap(res_id::PLAYER_BULLET));

		auto dynamics = cmp::create_const_velocity_dynamics(vx, vy);

		auto orientation = cmp::create_orientation(x, y, theta);

		auto movement_bounds = shared_ptr<cmp::bounds>();

		auto life_bounds = cmp::create_bounds(
			0.0, 0.0, _config.get_screen_w(), _config.get_screen_h());

		auto shape = cmp::create_circle(x, y, 8.0);

		auto coll_queue = cmp::create_coll_queue();

		shared_ptr<cmp::painmap> painmap;
		switch(cc) {
		case cmp::coll_class::PLAYER_BULLET:
			painmap = cmp::create_painmap({
				{ cmp::coll_class::ENEMY_SHIP, bullet_health } });
			break;
		case cmp::coll_class::ENEMY_BULLET:
			painmap = cmp::create_painmap({
				{ cmp::coll_class::PLAYER_SHIP, bullet_health } });
			break;
		default:
			throw string("Illegal collision class for a bullet.");
		}

		auto wellness = cmp::create_wellness(bullet_health);

		// Register nodes.
		_ds.add_node({id, appearance, orientation});

		_ms.add_node({	id,
				dynamics,
				orientation,
				shape,
				movement_bounds,
				life_bounds});
		
		_cs.add_node({ id, cc, shape, coll_queue });

		_ps.add_node({ id, cc, coll_queue, painmap, wellness });

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
			break;

		case comm::msg_t::spawn_bullet:
			create_bullet(msg.spawn_bullet.x,
					msg.spawn_bullet.y,
					msg.spawn_bullet.theta,
					msg.spawn_bullet.vx,
					msg.spawn_bullet.vy,
					msg.spawn_bullet.cc);
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
	, _last_id(0)
	{
		_keys[ALLEGRO_KEY_UP] = false;
		_keys[ALLEGRO_KEY_DOWN] = false;
		_keys[ALLEGRO_KEY_LEFT] = false;
		_keys[ALLEGRO_KEY_RIGHT] = false;
		_keys[ALLEGRO_KEY_LCTRL] = false;
		create_player_ship(200.0, 200.0);
		create_eye();
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

		// Update the systems.
		_ms.update(dt, _messages);
		_cs.update();
		_as.update(dt, _messages);
		_ds.update(dt);
		_ps.update(_messages);

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
