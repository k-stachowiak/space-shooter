#include "comm.h"

namespace comm {

message create_spawn_bullet(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy) {
	message msg;
	msg.type = msg_t::spawn_bullet;
	msg.spawn_bullet = { x, y, theta, vx, vy, enemy };
	return msg;
}

message create_spawn_missile(
		double x, double y,
		double theta,
		double vx, double vy,
		bool enemy) {
	message msg;
	msg.type = msg_t::spawn_missile;
	msg.spawn_missile = { x, y, theta, vx, vy, enemy };
	return msg;
}

message create_spawn_explosion(double x, double y) {
	message msg;
	msg.type = msg_t::spawn_explosion;
	msg.spawn_explosion = { x, y };
	return msg;
}

message create_spawn_smoke(double x, double y) {
	message msg;
	msg.type = msg_t::spawn_smoke;
	msg.spawn_smoke = { x, y };
	return msg;
}

message create_spawn_debris(
		double x, double y,
		double vx, double vy) {
	message msg;
	msg.type = msg_t::spawn_debris;
	msg.spawn_debris = { x, y, vx, vy };
	return msg;
}

message create_remove_entity(uint64_t id) {
	message msg;
	msg.type = msg_t::remove_entity;
	msg.remove_entity.id = id;
	return msg;
}

}
