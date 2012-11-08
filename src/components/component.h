#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdint.h>

#include <functional>
using namespace std;

#include "../config.h"
#include "../game_model.h"
#include "../collision/collision_space.h"

enum class drop_t {
	HEALTH,
	ARMOR,
};

enum class ent_msg_t {
	ANIMATION_END_REACHED,
	CANCEL_MOVE,	
	COLLISION,
	HEALTH_ETC,
	MOVE,
	ROTATE,
	ORIENTATION,	
	PAIN,
	PATH_END_REACHED,
	PICKUP,
};

struct ent_msg {
	ent_msg_t type;

	struct {
	} animation_end_reached;

	struct {
		bool cancel_dx;
		bool cancel_dy;
	} cancel_move;

	struct {
		collision_class cc;
	} collision;

	struct {
		double health;
		double armor;
		double max_health;
		double max_armor;
	} health_etc;

	struct {
		double dx;
		double dy;
	} move;

	struct {
		double dtheta;
	} rotate;

	struct {
		double x;
		double y;
		double theta;
	} orientation;

	struct {
		double amount;
	} pain;

	struct {
	} path_end_reached;

	struct {
		double health;
		double armor;
	} pickup;
};

class component {
protected:
	uint32_t _parent_id;

public:
	component(uint32_t parent_id)
		: _parent_id(parent_id) {
	}

	virtual ~component() {}

	virtual void initialize(vector<ent_msg>& ent_msgs) = 0;
	virtual void deinitialize(game_model& gm) = 0;

	virtual void action(
		const config& cfg,
		game_model& gm,
		vector<ent_msg>& ent_msgs) = 0;

	virtual void receive_message(
		const ent_msg& msg,
		vector<ent_msg>& ent_msgs) = 0;
};

#endif
