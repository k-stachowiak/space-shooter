#ifndef COLLISION_SPACE_H
#define COLLISION_SPACE_H

#include <stdint.h>

#include <functional>
#include <vector>
#include <memory>
#include <map>
using namespace std;

#include "shape.h"

enum class collision_class : uint32_t {
	PLAYER_SHIP,
	PLAYER_BULLET,
	ENEMY_SHIP,
	ENEMY_BULLET,
	HEALTH_PICKUP,
	ARMOR_PICKUP,
};

struct coll_report {
	collision_class class_a;
	shared_ptr<shape> shape_a;
	collision_class class_b;
	shared_ptr<shape> shape_b;
};

struct coll_entry {
	collision_class cc;
	shared_ptr<shape> shp;
	function<void(const coll_report&)> callback;
	coll_entry(collision_class cc, shared_ptr<shape> shape, function<void(const coll_report&)> callback)
		: cc(cc), shp(shape), callback(callback) {}
};

class collision_space {
	vector<coll_entry> _collision_entries;
public:
	void register_shape(collision_class cc, shared_ptr<shape> shp, function<void(const coll_report&)> cb);
	void check_collisions();
	void remove_entry_by_shape_id(uint32_t id);
	uint32_t num_entries() const { return _collision_entries.size(); }
};

#endif
