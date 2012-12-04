#ifndef STATE_H
#define STATE_H

#include <memory>
using namespace std;

#include "../config.h"
#include "../resources/resman.h"

class resources;

class state {
public:
	virtual ~state() {}

	// State transitions related.
	virtual void sigkill() = 0;
	virtual bool done() = 0;
	virtual unique_ptr<state> next_state() = 0;

	// Logic.
	virtual void frame_logic(double dt) = 0;

	// Optionally implementable.
	virtual void key_up(int k) {}
	virtual void key_down(int k) {}
};

unique_ptr<state> create_test_state(const config& config, const resman& res);

#endif
