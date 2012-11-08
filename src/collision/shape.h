#ifndef SHAPE_H
#define SHAPE_H

#include <stdint.h>

class circle;

class shape {
	uint32_t _id;

public:
	shape(uint32_t id) : _id(id) {
	}

	virtual ~shape() {}

	uint32_t get_id() const { return _id; }

	virtual void shift(double dx, double dy) = 0;

	virtual bool collide_with(const shape& s) const = 0;
	virtual bool collide_with_circle(const circle& c) const = 0;
};

#endif