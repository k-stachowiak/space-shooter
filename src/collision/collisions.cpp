#include "../geometry/misc.h"
#include "collisions.h"

bool collide_circle_circle(const circle& a, const circle& b) {
	double dx = b.get_x() - a.get_x();
	double dy = b.get_y() - a.get_y();
	double length = 1.0 / Q_rsqrt(dx * dx + dy * dy);
	return length < (a.get_r() + b.get_r());
}