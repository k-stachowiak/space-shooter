#ifndef COMM_H
#define COMM_H

#include <stdint.h>

namespace comm {

enum class msg_t {
	remove_entity,
	spawn_bullet,
	spawn_missile,
	spawn_explosion,
	spawn_smoke,
	spawn_debris
};

struct message {

	msg_t type;

	struct {
		uint64_t id;
	} remove_entity;

	struct {
		double x, y;
		double theta;
		double vx, vy;
		bool enemy;
	} spawn_bullet;

	struct {
		double x, y;
		double theta;
		double vx, vy;
		bool enemy;
	} spawn_missile;

	struct {
		double x, y;
	} spawn_explosion;

	struct {
		double x, y;
	} spawn_smoke;

	struct {
		double x, y;
		double vx, vy;
	} spawn_debris;
};

message create_spawn_bullet(double x, double y, double theta, double vx, double vy, bool enemy);
message create_spawn_missile(double x, double y, double theta, double vx, double vy, bool enemy);
message create_spawn_explosion(double x, double y);
message create_spawn_smoke(double x, double y);
message create_spawn_debris(double x, double y, double vx, double vy);
message create_remove_entity(uint64_t id);

}

#endif
