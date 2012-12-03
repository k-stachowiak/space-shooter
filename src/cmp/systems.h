#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <vector>
using std::vector;

#include <allegro5/allegro.h>

#include "nodes.h"
#include "comm.h"

template<class T>
inline bool between(T value, T min, T max) { return value >= min && value <= max; }

namespace sys {

template<typename SYS>
void remove_node(SYS& sys, uint64_t id) {
	for(auto n = begin(sys._nodes); n != end(sys._nodes); ++n) {
		if(n->identity == id) {
			*n = sys._nodes.back();
			sys._nodes.pop_back();
			--n;
		}
	}
}

class system {
protected:
	bool _debug_mode;
public:
	void set_debug_mode(bool debug_mode) { _debug_mode = debug_mode; }
};

class drawing_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::drawing_node> _nodes;
public:
	void add_node(nd::drawing_node n) { _nodes.push_back(n); }
	void update(double dt);
};

class fx_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::fx_node> _nodes;
public:
	void add_node(nd::fx_node n) { _nodes.push_back(n); }
	void update(double dt, vector<comm::message>& msgs);
};

class movement_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::movement_node> _nodes;
	uint64_t _player_controlled;
	double _player_throttle_x;
	double _player_throttle_y;
public:
	movement_system() : _player_throttle_x(0.0) , _player_throttle_y(0.0) {}
	void add_node(nd::movement_node n) { _nodes.push_back(n); }
	void set_player_controlled(uint64_t id) { _player_controlled = id; }
	void set_player_throttle(double x, double y) { _player_throttle_x = x; _player_throttle_y = y; }
	void update(double dt, vector<comm::message>& msgs);
};

class arms_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::arms_node> _nodes;
	uint64_t _player_shooting;
	double _player_interval;
	bool _player_trigger;
	double _player_counter;
	void handle_player(double dt, vector<comm::message>& msgs, double x, double y);
	comm::message proc_msg(double x, double y, comm::message msg);
public:
	arms_system() : _player_counter(0.0) {}
	void add_node(nd::arms_node const& n) { _nodes.push_back(n); }
	void set_player_shooting(uint64_t id) { _player_shooting = id; }
	void set_player_interval(double interval) { _player_interval = interval; }
	void set_player_trigger(bool trigger) { _player_trigger = trigger; }
	void update(double dt, vector<comm::message>& msgs);
};

class collision_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::collision_node> _nodes;
	void check_collision(nd::collision_node const& a, nd::collision_node const& b);
public:
	void add_node(nd::collision_node node) { _nodes.push_back(node); }
	void update();
};

class pain_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::pain_node> _nodes;
public:
	void add_node(nd::pain_node node) { _nodes.push_back(node); }
	void update(vector<comm::message>& msgs);
};

class wellness_system : public system {
	template<typename SYS> friend void remove_node(SYS&, uint64_t);
	vector<nd::wellness_node> _nodes;
public:
	void add_node(nd::wellness_node node) { _nodes.push_back(node); }
	void update(double dt, vector<comm::message>& msgs);
};

}

#endif
