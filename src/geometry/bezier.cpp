/* Copyright (C) 2012 Krzysztof Stachowiak */

/*
* This file is part of space-shooter.
*
* space-shooter is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* space-shooter is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with space-shooter; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "bezier.h"

static inline double bezier_interpolate_value(double x0, double x1, double step) {
        return x0 + step * (x1 - x0);
}

static inline point bezier_interpolate_segment(const segment& s, double step) {
        double x = bezier_interpolate_value(s.a.x, s.b.x, step);
        double y = bezier_interpolate_value(s.a.y, s.b.y, step);
        return point(x, y);
}

static inline std::vector<point> bezier_interpolate_points(const std::vector<segment>& segments, double step) {
        std::vector<point> result;
        for(auto& s : segments) {
                result.push_back(bezier_interpolate_segment(s, step));
        }
        return result;
}

static inline std::vector<segment> bezier_gen_segments(const std::vector<point>& points) {
        std::vector<segment> result;
        for(uint32_t i = 1; i < points.size(); ++i) {
                result.emplace_back(points[i - 1], points[i]);
        }
        return result;
}

std::vector<point> bezier(const std::vector<point>& points, uint32_t num_midpoints) {

        std::vector<point> result;

        for(uint32_t i = 0; i < num_midpoints; ++i) {

                // Compute normalized step.
                double step = (double)i / (double)num_midpoints;

                // Collapse all the segments.
                std::vector<segment> segments = bezier_gen_segments(points);
                while (segments.size() > 1) {
                        std::vector<point> new_points = bezier_interpolate_points(segments, step);
                        segments = bezier_gen_segments(new_points);
                }

                // Interpolate the final segment
                result.push_back(bezier_interpolate_segment(segments.front(), step));
        }

        return result;
}
