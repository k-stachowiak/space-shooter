#include "cmp_shoot_player.h"

cmp_shoot_player::cmp_shoot_player(uint32_t parent_id, double period, double max_x_dev)
	: component(parent_id)
	, _accumulator(0.0)
	, _period(period)
	, _max_x_dev(max_x_dev) {
}

void cmp_shoot_player::initialize(vector<ent_msg>& ent_msgs) {
	_previous_left = false;
}

void cmp_shoot_player::action(const config& cfg, game_model& gm, vector<ent_msg>& ent_msgs) {

	if(!gm.trigger_down) {
		// Set to period so that next time player shoots immediately
		// after pressing the trigger.
		_accumulator = _period;
		return;
	}

	_accumulator += gm.last_dt;
	if(_accumulator >= _period) {

		_accumulator -= _period;

		uniform_real_distribution<double> x_dev_dist(0.0, _max_x_dev);
		double x_dev = x_dev_dist(_engine);

		if(_previous_left) {
			gm.game_msgs.push_back(game_msg::create_spawn_bullets(
				_last_x + 10.0,
				_last_y,
				x_dev,
				-cfg.get_player_bullet_v(),
				res_id::PLAYER_BULLET,
				false));
			_previous_left = false;

		} else {
			gm.game_msgs.push_back(game_msg::create_spawn_bullets(
				_last_x - 10.0,
				_last_y,
				-x_dev,
				-cfg.get_player_bullet_v(),
				res_id::PLAYER_BULLET,
				false));
			_previous_left = true;

		}
	}
}

void cmp_shoot_player::receive_message(const ent_msg& msg, vector<ent_msg>& ent_msgs) {
	switch(msg.type) {
	case ent_msg_t::ORIENTATION:
		_last_x = msg.orientation.x;
		_last_y = msg.orientation.y;
		break;

	default:
		break;
	}
}
