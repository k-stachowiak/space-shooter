#ifndef CMP_COLLIDABLE_H
#define CMP_COLLIDABLE_H

#include "component.h"

#include "../collision/shape.h"
#include "../collision/collision_space.h"

class cmp_collidable : public component {

	// Properties.
	collision_class _class;
	shared_ptr<shape> _shp;
	collision_space& _cs;

	// State.
	vector<coll_report> _reports;

	ent_msg handle_report(const coll_report& report);

public:
	cmp_collidable(
		uint32_t parent_id,
		collision_class cc,
		shared_ptr<shape> shp,
		collision_space& cs);

	void callback(const coll_report& report);

	void initialize(vector<ent_msg>& ent_msgs) {}
	void deinitialize(game_model& gm);

	void action(const config& cfg, game_model& gs, vector<ent_msg>& ent_msgs);
	void receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs);
};

#endif
