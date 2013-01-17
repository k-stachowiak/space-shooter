#include "../misc/rand.h"
#include "../geometry/bezier.h"
#include "entity_factory.h"

uint64_t entity_factory::create_explosion(double x, double y) {

	// Initialize components.
	uint64_t id = ++_last_id;
	
	// TODO: Add an animation "constructor" that will automate this
	// procedure below.
	uint32_t frame_width = 70;
	uint32_t num_frames = 16;
	double frame_time = 0.05;
	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(res_id::EXPLOSION),
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
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_smoke(double x, double y, comm::smoke_size size) {

	// Initialize components.
	uint64_t id = ++_last_id;

	res_id rid;
	double scale;
	switch(size) {
		case comm::smoke_size::small:
			rid = res_id::SMOKE_SMALL;
			scale = 0.5;
			break;
		case comm::smoke_size::medium:
			rid = res_id::SMOKE;
			scale = 1.0;
			break;
		case comm::smoke_size::big:
			rid = res_id::SMOKE_BIG;
			scale = 2.0;
			break;
		default:
			throw;
	}
	
	uint32_t frame_width = 32 * 3 * scale;
	uint32_t num_frames = 25;
	double frame_time = 0.075;
	vector<cmp::frame_def> frame_defs;
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, frame_time);

	auto appearance = cmp::create_simple_anim(
			_resman.get_bitmap(rid),
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
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_debris(double x, double y, double bvx, double bvy) {

	// Prepare helpers.
	vector<res_id> bitmaps {
		res_id::DEBRIS1,
		res_id::DEBRIS2,
		res_id::DEBRIS3,
		res_id::DEBRIS4,
		res_id::DEBRIS5 };

	// Bitmap selection.
	static uniform_int_distribution<int> bmp_dist(0, bitmaps.size() - 1);
	uint32_t index = bmp_dist(rnd::engine);
	res_id bmp = bitmaps[index];

	// TTL generation.
	static uniform_real_distribution<double> ttl_dist(1.0, 3.0);
	double ttl_time = ttl_dist(rnd::engine);

	// Random movement.
	bernoulli_distribution dir_dist;
	
	static uniform_real_distribution<double> mv_dist(100.0, 300.0);
	double base_vx = mv_dist(rnd::engine);
	double base_vy = mv_dist(rnd::engine);
	double mul_vx = dir_dist(rnd::engine) ? 1.0 : -1.0;
	double mul_vy = dir_dist(rnd::engine) ? 1.0 : -1.0;

	static uniform_real_distribution<double> rot_dist(40.0, 70.0);
	double base_av = rot_dist(rnd::engine);
	double mul_av = dir_dist(rnd::engine) ? 1.0 : -1.0;

	// Initialize components.
	uint64_t id = ++_last_id;
	
	auto appearance = cmp::create_static_bmp(_resman.get_bitmap(bmp));

	vector<shared_ptr<cmp::dynamics>> dynamics {
		cmp::create_const_velocity_dynamics(
				bvx + base_vx * mul_vx,
				bvy + base_vy * mul_vy),
		cmp::create_const_ang_vel_dynamics(
				base_av * mul_av)
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
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds });

	// Feedback for the state.
	return id;
}

uint64_t entity_factory::create_player_ship(double x, double y) {

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
		cmp::create_smoke_when_hurt(0.25)
	};

	bool explodes = true; 
	uint32_t num_debris = 10;

	// Register nodes.
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });

	_movement_system.set_player_controlled(id);
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});

	_arms_system.add_node({id, orientation, weapon_beh});
	_arms_system.set_player_shooting(id);
	_arms_system.set_player_interval(0.125);

	_collision_system.add_node({ id, id, cc, shape, coll_queue });

	_pain_system.add_node({ id, cc, coll_queue, painmap, wellness });

	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

	_fx_system.add_node({ id, orientation, wellness, fxs });

	return id;
}

uint64_t entity_factory::create_bomber() {

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
		cmp::create_smoke_when_hurt(0.25)
	};

	bool explodes = true; 
	uint32_t num_debris = 7;

	auto sc = cmp::score_class::ENEMY_BOMBER;

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, cc, coll_queue, painmap, wellness });
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_eye() {
	
	// Prepare resources for the components.
	// -------------------------------------

	// The animation.
	uint32_t frame_width = 32;
	uint32_t num_frames = 6;
	double blink_time = 0.15;
	double angry_time = 0.15;
	vector<cmp::frame_def> frame_defs {
		{ 4, angry_time },
		{ 3, angry_time },
		{ 2, blink_time }, // Close.
		{ 1, blink_time },
		{ 0, blink_time }, // Open.
		{ 1, blink_time },
		{ 2, blink_time }, // Close.
		{ 1, blink_time },
		{ 0, blink_time }, // Open.
		{ 1, blink_time },
		{ 2, blink_time },
		{ 3, angry_time }, // Angry
		{ 4, angry_time },
		{ 5, angry_time }
	};

	// Compute the animation cycle time.
	double anim_period = 0;
	for(auto const& fd : frame_defs)
		anim_period += fd.time;

	// The path.
	uniform_real_distribution<double> from_to_dist(
			0.0, _config.get_screen_w());

	uniform_real_distribution<double> pan_dist(
			0.0, _config.get_screen_w());

	bernoulli_distribution right_to_left(0.5);

	vector<point> points {
		{ from_to_dist(rnd::engine), 0.0 },
		{ 0.0,                       pan_dist(rnd::engine) },
		{ _config.get_screen_w(),    pan_dist(rnd::engine) },
		{ from_to_dist(rnd::engine), _config.get_screen_h() + 50 }
	};

	if(right_to_left(rnd::engine))
		swap(points[2], points[1]);
	
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
		cmp::create_smoke_when_hurt(0.25)
	};

	bool explodes = true; 
	uint32_t num_debris = 5;

	auto sc = cmp::score_class::ENEMY_EYE;

	// Register the components.
	// ------------------------
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics });
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds});
	_arms_system.add_node({ id, orientation, weapon_beh });
	_collision_system.add_node({ id, id, cc, shape, coll_queue });
	_pain_system.add_node({ id, cc, coll_queue, painmap, wellness });
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });
	_fx_system.add_node({ id, orientation, wellness, fxs });
	_score_system.add_node({ id, sc, wellness });

	return id;
}

uint64_t entity_factory::create_missile(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy,
		uint64_t origin_id) {

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
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds}); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue });
	_pain_system.add_node({ id, cc, coll_queue, painmap, wellness }); 
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl }); 
	_fx_system.add_node({ id, orientation, wellness, fxs });

	return id;
}

uint64_t entity_factory::create_bullet(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy,
		uint64_t origin_id) {

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
	_drawing_system.add_node({ id, appearance, orientation, shape, dynamics }); 
	_movement_system.add_node({ id, dynamics, orientation, shape, movement_bounds, life_bounds }); 
	_collision_system.add_node({ id, origin_id, cc, shape, coll_queue }); 
	_pain_system.add_node({ id, cc, coll_queue, painmap, wellness }); 
	_wellness_system.add_node({ id, explodes, num_debris, orientation, dynamics, wellness, ttl });

	return id;
}
