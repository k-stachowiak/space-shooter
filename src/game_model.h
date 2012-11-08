#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include <stdint.h>

#include <vector>
#include <tuple>
using namespace std;

#include "resources/resman.h"

enum class game_msg_t {
	REMOVE_ENTITY,
	SPAWN_BULLET,
	SPAWN_EXPL,
	SPAWN_SMOKE,
	SPAWN_HEALTH,
};

struct game_msg {

	// Type inticator.
	// ---------------
	game_msg_t type;

	// Sub-type data.
	// --------------
	struct {
		uint32_t id;
	} remove_entity;

	struct {
		double x, y;
		double vx, vy;
		res_id bitmap_id;
		bool is_enemy;
	} spawn_bullet;

	struct {
		uint32_t num_debris;
		double x, y;
		double vx, vy;
	} spawn_expl;

	struct {
		double x, y;
	} spawn_smoke;

	struct {
		double x, y;
		double vx, vy;
	} spawn_health;

	// Constructors.
	// -------------
	static game_msg create_remove_entity(uint32_t id) {
		game_msg result;
		result.type = game_msg_t::REMOVE_ENTITY;
		result.remove_entity.id = id;
		return result;
	}

	static game_msg create_spawn_bullets(
			double x, double y,
			double vx, double vy,
			res_id bitmap_id,
			bool is_enemy) {
		game_msg result;
		result.type = game_msg_t::SPAWN_BULLET;
		result.spawn_bullet = {
			x, y, vx, vy,
			bitmap_id, is_enemy };
		return result;
	}

	static game_msg create_spawn_expl(
			uint32_t num_debris,
			double x, double y,
			double vx, double vy) {
		game_msg result;
		result.type = game_msg_t::SPAWN_EXPL;
		result.spawn_expl = {
			num_debris,
			x, y, vx, vy };
		return result;
	}

	static game_msg create_spawn_smoke(double x, double y) {
		game_msg result;
		result.type = game_msg_t::SPAWN_SMOKE;
		result.spawn_smoke = { x, y };
		return result;
	}

	static game_msg create_spawn_health(
			double x, double y,
			double vx, double vy) {
		game_msg result;
		result.type = game_msg_t::SPAWN_HEALTH;
		result.spawn_health = { x, y, vx, vy };
		return result;
	}
};

struct game_model {

	game_model()
	: debug(false)
	, last_dt(-1.0)
	, steering_vec { 0.0 , 0.0 }
	, trigger_down(false)
	, score(0.0)
	, player_health(0.0)
	{}

	// Is the debug data requested.
	bool debug;

	// Tempus fugit.
	double last_dt;

	// Player's data.
	double steering_vec[2];
	bool trigger_down;
	double score;
	double player_health;
	double player_armor;

	// Core game messages queue.
	vector<game_msg> game_msgs;
};

#endif
