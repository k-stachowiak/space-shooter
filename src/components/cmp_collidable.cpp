#include <functional>
using namespace std::placeholders;

#include "cmp_collidable.h"

ent_msg cmp_collidable::handle_report(const coll_report& report) {

	collision_class other_class;

	// Check, what is the class of the otherobject from the collision.
	if(report.class_a == _class) {
		other_class = report.class_b;
	} else {
		other_class = report.class_a;
	}

	// Report the collision.
	ent_msg msg;
	msg.type = ent_msg_t::COLLISION;
	msg.collision.cc = other_class;
	return msg;
}

cmp_collidable::cmp_collidable(
	uint32_t parent_id,
	collision_class cc,
	shared_ptr<shape> shp,
	collision_space& cs) 
: component(parent_id)
, _class(cc)
, _shp(shp)
, _cs(cs) {
	cs.register_shape(cc, shp, bind(&cmp_collidable::callback, this, _1));
}

void cmp_collidable::callback(const coll_report& report) {
	_reports.push_back(report);
}

void cmp_collidable::deinitialize(game_model& gm) {
	_cs.remove_entry_by_shape_id(_shp->get_id());
}

void cmp_collidable::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {
	while(!_reports.empty()) {
		ent_msgs.push_back(handle_report(_reports.back()));
		_reports.pop_back();
	}
}

void cmp_collidable::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::MOVE:
		_shp->shift(msg.move.dx, msg.move.dy);
		break;
	default:
		break;
	}
}
