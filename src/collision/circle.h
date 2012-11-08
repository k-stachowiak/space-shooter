#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

class circle : public shape {
	double _x;
	double _y;
	double _r;
public:
	circle(uint32_t id, double x, double y, double r);

	double get_x() const { return _x; }
	double get_y() const { return _y; }
	double get_r() const { return _r; }

	void shift(double dx, double dy) { _x += dx; _y += dy; }

	bool collide_with(const shape& s) const;
	bool collide_with_circle(const circle& c) const;
};

#endif