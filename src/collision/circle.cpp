#include "collisions.h"
#include "circle.h"

circle::circle(uint32_t id, double x, double y, double r)
	: shape(id)
	, _x(x)
	, _y(y)
	, _r(r) {
}

bool circle::collide_with(const shape& s) const {
	return s.collide_with_circle(*this);
}

bool circle::collide_with_circle(const circle& c) const {
	return collide_circle_circle(*this, c);
}