#ifndef COMM_H
#define COMM_H

#include "components.h"

namespace comm {

enum class msg_t {
	remove_entity,
	spawn_bullet
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
		cmp::coll_class cc;
	} spawn_bullet;
};

message create_spawn_bullet( double x, double y, double theta, double vx, double vy, cmp::coll_class cc);
message create_remove_entity(uint64_t id);

}

#endif
