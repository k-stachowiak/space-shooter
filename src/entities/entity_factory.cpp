#include <cmath>
using namespace std;

#include "../geometry/bezier.h"
#include "../collision/circle.h"
#include "entity_factory.h"
#include "entity_builder.h"

entity_factory::entity_factory(
	const config& config,
	const resman& resman)
: _config(config)
, _resman(resman)
, _last_id(0)
, _lase_shape_id(0) {
}

entity entity_factory::create_player_ship(
		double x, double y,
		double screen_w, double screen_h,
		double theta,
		double shoot_period,
		double health,
		double armor,
		collision_space& coll_sp) {

	// Prepare the pain map.
	// ---------------------
	map<collision_class, double> pain_map;
	pain_map[collision_class::ENEMY_BULLET] = 5.0;

	// Prepare a shape for the collisions.
	shared_ptr<shape> shp(new circle(++_lase_shape_id, x, y, 32.0));

	// Build the entity.
	entity_builder eb(_resman);
	entity player_ship =
		eb.begin(++_last_id).
		add_appr_bitmap(res_id::PLAYER_SHIP).
		add_move_player().
		add_bounded_movement(0.0, screen_w, 0.0, screen_h).
		add_orientation(x, y, theta).
		add_shoot_player(shoot_period, 75.0).
		add_health_etc(health, armor).
		add_painmap(pain_map).
		add_player_health().
		add_smoke_when_hurt().
		add_explode_when_die(15).
		add_collidable(collision_class::PLAYER_SHIP, shp, coll_sp).
		add_health_picker(10.0, 0.0).
		build();

	player_ship.initialize_components();

	return player_ship;
}

entity entity_factory::create_bullet(
		double x,
		double y,
		double vx,
		double vy,
		double screen_w,
		double screen_h,
		res_id image_id,
		collision_space& coll_sp,
		collision_class coll_class) {

	static const double health = 1.0;
	static const double pain = 2.0; // Make sure this is greater than health.

	// Prepare the pain map.
	// ---------------------
	map<collision_class, double> pain_map;
	switch(coll_class) {
	case collision_class::ENEMY_BULLET:
		pain_map[collision_class::PLAYER_SHIP] = pain;
		break;
	case collision_class::PLAYER_BULLET:
		pain_map[collision_class::ENEMY_SHIP] = pain;
		break;
	default:
		throw;
	}		

	// Prepare shape.
	// --------------
	shared_ptr<shape> shp(new circle(++_lase_shape_id, x, y, 8.0));

	// Build the entity.
	// -----------------
	entity_builder eb(_resman);
	entity bullet = eb.begin(++_last_id).
		add_appr_bitmap(image_id).
		add_move_const_speed(vx, vy).
		add_orientation(x, y, atan2(vy, vx)).
		add_bounded_life(0.0, screen_w, 0.0, screen_h).
		add_health_etc(health, 0.0).
		add_painmap(pain_map).
		add_collidable(coll_class, shp, coll_sp).
		build();
		
	bullet.initialize_components();

	return bullet;
}

entity entity_factory::create_star(double x, double y, double near_v, double zdist, double screen_w, double screen_h) {

	double grade = 1.0 + 0.9 * zdist;

	double r = grade;
	double g = grade;
	double b = grade;
	double a = 1.0;

	double vx = 0.0;
	double vy = grade * near_v;

	entity_builder eb(_resman);
	entity star = eb.begin(++_last_id).
		add_appr_pixel(r, g, b, a).
		add_move_const_speed(vx, vy).
		add_orientation(x, y, 0.0).
		add_bounded_life(0.0, screen_w, 0.0, screen_h).
		build();

	star.initialize_components();

	return star;
}

entity entity_factory::create_enemy_eye(
		double screen_w, 
		double screen_h,
		collision_space& coll_sp) {

	// Prepare the pain map.
	// ---------------------
	map<collision_class, double> pain_map;
	pain_map[collision_class::PLAYER_BULLET] = 10.0;

	// Prepare the animation.
	// ----------------------
	uint32_t frame_width = 32;
	uint32_t num_frames = 6;
	double blink_time = 0.15;
	double angry_time = 0.15;

	vector<frame_def> frame_defs;	
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

	// Prepare the path.
	// -----------------
	
	uniform_real_distribution<double> from_to_dist(0.0, screen_w);
	uniform_real_distribution<double> pan_dist(0.0, screen_w);
	bernoulli_distribution right_to_left(0.5);

	vector<point> points;

	// Starting point.
	points.emplace_back(from_to_dist(_engine), 0.0);

	// Pan to the left and right.
	points.emplace_back(0.0, pan_dist(_engine));
	points.emplace_back(screen_w, pan_dist(_engine));
	if(right_to_left(_engine)) swap(points[2], points[1]);

	// Ending point.
	points.emplace_back(from_to_dist(_engine), screen_h + 50);

	// Smoothen path.
	points = bezier(points, 50);

	// Prepare collision shape.
	// ------------------------
	shared_ptr<shape> shp(new circle(
		++_lase_shape_id, 
		points.front().x, points.front().y,
		16.0));

	// Initialize the objects.
	// -----------------------		
	entity_builder eb(_resman);
	entity eye = eb.begin(++_last_id).
		add_appr_simple_anim(res_id::ENEMY_EYE, frame_width, num_frames, frame_defs, -1).
		add_move_path(points).
		add_orientation(points.front().x, points.front().y, 0.0).
		add_bounded_life(0.0, screen_w, 0.0, screen_h).
		add_bullet_emitter(anim_period, anim_period, _config.get_eye_bullet_v(), res_id::EYE_BULLET, true).
		add_health_etc(33.3, 20.0).
		add_explode_when_die(5).
		add_drop_when_die(drop_t::HEALTH).
		add_score_when_die(1).
		add_painmap(pain_map).
		add_collidable(collision_class::ENEMY_SHIP, shp, coll_sp).
		build();

	eye.initialize_components();

	return eye;
}

entity entity_factory::create_explosion(double x, double y, double vx, double vy) {

	// Prepare the animation.
	// ----------------------
	uint32_t frame_width = 70;
	uint32_t num_frames = 16;

	vector<frame_def> frame_defs;	
	for(uint32_t i = 0; i < num_frames; ++i) {
		frame_defs.emplace_back(i, 0.05);
	}

	// Initialize the objects.
	// -----------------------	
	entity_builder eb(_resman);
	entity explosion = eb.begin(++_last_id).
		add_appr_simple_anim(res_id::EXPLOSION_1, frame_width, num_frames, frame_defs, 1).
		add_orientation(x, y, 0.0).
		add_move_const_speed(vx, vy).
		add_die_end_anim().
		build();

	explosion.initialize_components();

	return explosion;
}

entity entity_factory::create_smoke(double x, double y) {

	// Prepare the animation.
	// ----------------------
	uint32_t frame_width = 32 * 3;
	uint32_t num_frames = 25;

	vector<frame_def> frame_defs;	
	for(uint32_t i = 0; i < num_frames; ++i)
		frame_defs.emplace_back(i, 0.075);

	// Initialize the objects.
	// -----------------------	
	entity_builder eb(_resman);
	entity smoke = eb.begin(++_last_id).
		add_appr_simple_anim(res_id::SMOKE_1, frame_width, num_frames, frame_defs, 1).
		add_orientation(x, y, 0.0).
		add_die_end_anim().
		build();

	smoke.initialize_components();

	return smoke;
}

entity entity_factory::create_debris(double x, double y, double vx, double vy) {

	// Randomly generate some of the parameters.
	// -----------------------------------------
	uniform_real_distribution<double> ttl_dist(0.5, 2.0);
	double ttl = ttl_dist(_engine);

	uniform_real_distribution<double> dtheta_dist(3.1415 * 1.0, 3.1415 * 3.0);
	double dtheta = dtheta_dist(_engine);

	vector<res_id> debris_images {
		res_id::DEBRIS1,
		res_id::DEBRIS2,
		res_id::DEBRIS3,
		res_id::DEBRIS4,
		res_id::DEBRIS5	};

	uniform_int_distribution<size_t>
		debris_res_id_dist(0, debris_images.size() - 1);

	res_id debris_res_id = debris_images[debris_res_id_dist(_engine)];

	uniform_real_distribution<double> vel_dist(-250.0, 250.0);
	double dvx = vel_dist(_engine);
	double dvy = vel_dist(_engine);

	// Construct the object.
	// ---------------------
	entity_builder eb(_resman);
	entity debris = eb.begin(++_last_id).
		add_appr_bitmap(debris_res_id).
		add_time_to_live(ttl).
		add_orientation(x, y, 0.0).
		add_move_const_speed(vx + dvx, vy + dvy).
		add_rotate_const_speed(dtheta).
		// add_explode_when_die(0).
		build();

	debris.initialize_components();

	return debris;
}

entity entity_factory::create_health_pickup(
		double x, 
		double y, 
		double vx, 
		double vy,
		double screen_w,
		double screen_h,
		collision_space& coll_sp) {

	double dtheta = 3.1415;

	// Prepare the pain map.
	// ---------------------
	map<collision_class, double> pain_map;
	pain_map[collision_class::PLAYER_SHIP] = 2.0;

	// Prepare a shape for the collisions.
	shared_ptr<shape> shp(new circle(++_lase_shape_id, x, y, 16.0));

	// Construct the object.
	// ---------------------
	entity_builder eb(_resman);
	entity hp = eb.begin(++_last_id).
		add_appr_bitmap(res_id::HEALTH).
		add_bounded_life(0.0, screen_w, 0.0, screen_h).
		add_orientation(x, y, 0.0).
		add_move_const_speed(vx, vy).
		add_rotate_const_speed(dtheta).
		add_painmap(pain_map).
		add_collidable(collision_class::HEALTH_PICKUP, shp, coll_sp).
		add_health_etc(1.0, 0.0).
		build();

	hp.initialize_components();

	return hp;
}
