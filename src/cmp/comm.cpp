#include "comm.h"

namespace comm {

message create_spawn_bullet(
		double x, double y,
		double theta,
		double vx, double vy,
		cmp::coll_class cc) {
	message msg;
	msg.type = msg_t::spawn_bullet;
	msg.spawn_bullet = { x, y, theta, vx, vy, cc };
	return msg;
}

message create_remove_entity(uint64_t id) {
	message msg;
	msg.type = msg_t::remove_entity;
	msg.remove_entity.id = id;
	return msg;
}

}
