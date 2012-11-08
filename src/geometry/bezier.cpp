#include "bezier.h"

static inline double bezier_interpolate_value(double x0, double x1, double step) {
	return x0 + step * (x1 - x0);
}

static inline point bezier_interpolate_segment(const segment& s, double step) {
	double x = bezier_interpolate_value(s.a.x, s.b.x, step);
	double y = bezier_interpolate_value(s.a.y, s.b.y, step);
	return point(x, y);
}

static inline vector<point> bezier_interpolate_points(const vector<segment>& segments, double step) {
	vector<point> result;
	for(auto& s : segments) {
		result.push_back(bezier_interpolate_segment(s, step));
	}
	return result;
}

static inline vector<segment> bezier_gen_segments(const vector<point>& points) {
	vector<segment> result;
	for(uint32_t i = 1; i < points.size(); ++i) {
		result.emplace_back(points[i - 1], points[i]);
	}
	return result;
}

vector<point> bezier(const vector<point>& points, uint32_t num_midpoints) {

	vector<point> result;

	for(uint32_t i = 0; i < num_midpoints; ++i) {

		// Compute normalized step.
		double step = (double)i / (double)num_midpoints;

		// Collapse all the segments.
		vector<segment> segments = bezier_gen_segments(points);
		while (segments.size() > 1) {
			vector<point> new_points = bezier_interpolate_points(segments, step);
			segments = bezier_gen_segments(new_points);
		}

		// Interpolate the final segment
		result.push_back(bezier_interpolate_segment(segments.front(), step));
	}

	return result;
}