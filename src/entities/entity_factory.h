#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <stdint.h>

#include <random>
using std::default_random_engine;

#include <allegro5/allegro.h>

#include "../resources/resman.h"
#include "../entities/entity.h"
#include "../collision/collision_space.h"

class entity_factory {

	default_random_engine _engine;

	const config& _config;
	const resman& _resman;
	uint32_t _last_id;
	uint32_t _lase_shape_id;

public:
	entity_factory(const config& config, const resman& resman);
	entity create_player_ship(double x, double y, double screen_w, double screen_h, double theta, double shoot_period, double health, double armor, collision_space& coll_sp);
	entity create_bullet(double x, double y, double vx, double vy, double screen_w, double screen_h, res_id image_id, collision_space& coll_sp, collision_class coll_class);
	entity create_star(double x, double y, double near_v, double zdist, double screen_w, double screen_h);
	entity create_enemy_eye(double screen_w, double screen_h, collision_space& coll_sp);
	entity create_explosion(double x, double y, double vx, double vy);
	entity create_smoke(double x, double y);
	entity create_debris(double x, double y, double vx, double vy);
	entity create_health_pickup(double x, double y, double vx, double vy, double screen_w, double screen_h, collision_space& coll_sp);
};

#endif
