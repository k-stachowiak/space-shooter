#ifndef RANDOM_CLOCK_H
#define RANDOM_CLOCK_H

#include <random>
using std::default_random_engine;

#include <functional>
using std::function;

template<class DISTRIBUTION>
class random_clock {

	default_random_engine _engine;
	DISTRIBUTION _distribution;
	function<void()> _callback;
	double _timer;

	void schedule(double remainder) {
		_timer = _distribution(_engine);
		_timer -= remainder;
	}

public:
	random_clock(const DISTRIBUTION& distribution, function<void()> callback)
		: _distribution(distribution)
		, _callback(callback) {
			schedule(0.0);
	}

	void tick(double dt) {
		_timer -= dt;
		if(_timer <= 0.0) {
			schedule(-_timer);
			_callback();
		}
	}
};

#endif