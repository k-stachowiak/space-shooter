#ifndef TYPES_H
#define TYPES_H

struct point {
	double x, y;
	point(double x, double y) : x(x), y(y) {}
};

struct segment {
	point a, b;
	segment(const point& a, const point& b) : a(a), b(b) {}
};

#endif