#ifndef NODES_H
#define NODES_H

#include <memory>
using std::shared_ptr;

#include "components.h"

namespace nd {

/*
 * Note that the read only properties may be stored by value.
 * However all the other properties must be stored here as 
 * pointers as the changes made by one system must be visible
 * in another one.
 */

struct drawing_node {
	uint64_t identity;
	shared_ptr<cmp::appearance> appearance;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	vector<shared_ptr<cmp::dynamics>> dynamics;
};

struct fx_node {
	uint64_t identity;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::wellness> wellness;
	vector<shared_ptr<cmp::fx>> effects;
};

struct movement_node {
	uint64_t identity;
	vector<shared_ptr<cmp::dynamics>> dynamics;
	shared_ptr<cmp::orientation> orientation;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::bounds> movement_bounds;
	shared_ptr<cmp::bounds> life_bounds;
};

struct arms_node {
	uint64_t identity;
	shared_ptr<cmp::orientation> orientation;
	vector<shared_ptr<cmp::weapon_beh>> weapon_beh;
};

struct collision_node {
	uint64_t identity;
	cmp::coll_class cc;
	shared_ptr<cmp::shape> shape;
	shared_ptr<cmp::coll_queue> coll_queue;
};

struct pain_node {
	uint64_t identity;
	cmp::coll_class cc;
	shared_ptr<cmp::coll_queue> coll_queue;
	shared_ptr<cmp::painmap> painmap;
	shared_ptr<cmp::wellness> wellness;
};

struct wellness_node {
	uint64_t identity;
	bool explodes;
	uint32_t num_debris;
	shared_ptr<cmp::orientation> orientation;
	vector<shared_ptr<cmp::dynamics>> dynamics;
	shared_ptr<cmp::wellness> wellness;
	shared_ptr<cmp::timer> ttl;
};

}

#endif
