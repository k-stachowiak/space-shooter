/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#include <cmath>
#include <tuple>

#include "shape.h"

namespace
{

int collide_segment_segment_impl(
                double ax1, double ay1, double ax2, double ay2,
                double bx1, double by1, double bx2, double by2,
                std::vector<efwk::point>& result)
{
        const double sx1 = bx1 - ax1;
        const double sy1 = by1 - ay1;
        const double sx2 = bx2 - ax2;
        const double sy2 = by2 - ay2;

        const float s = (-sy1 * (ax1 - ax2) + sx1 * (ay1 - ay2)) / (-sx2 * sy1 + sx1 * sy2);
        const float t = (sx2 * (ay1 - ay2) - sy2 * (ax1 - ax2)) / (-sx2 * sy1 + sx1 * sy2);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
                result.push_back({ ax1 + (t * sx1), ay1 + (t * sy1) });
                return 1;
        } else {
                return 0;
        }
}

int collide_segment_circle_impl(
                double x1, double y1, double x2, double y2,
                double xc, double yc, double r,
                std::vector<efwk::point>& result)
{
        // 0.5 means half of pixel.
        static const double epsilon = 0.5;

        // The segment's equations:
        // x = x1 * t + x2 * (1 - t)        (1)
        // y = y1 * t + y2 * (1 - t)        (2)

        // The circle's equation:
        // (x - xc)^2 + (y - yc)^2 = r^2    (3)

        // After substituting (1) and (2) in (3) we obtain
        // a quadratic equation in the function of t

        // The solution will tell us two things:
        // - the delta will tell us whether the line and the circle intersect
        // - the values will tell us if the intersections lie within the segment:
        //   t in <0, 1> means that the intersection point lies in the segment.

#define SQR(x) ((x) * (x))

        const double a = SQR(x1) - 2 * x1 * x2 + SQR(x2) +
                         SQR(y1) - 2 * y1 * y2 + SQR(y2);

        const double b = 2 * x2 * xc - 2 * x1 * xc - 2 * SQR(x2) + 2 * x1 * x2 +
                         2 * y2 * yc - 2 * y1 * yc - 2 * SQR(y2) + 2 * y1 * y2;

        const double c = SQR(x2) - 2 * x2 * xc + SQR(xc) +
                         SQR(y2) - 2 * y2 * yc + SQR(yc) -
                         SQR(r);

        const double delta = SQR(b) - 4 * a * c;

#undef SQR

        // No results at all.
        if (delta < -epsilon) {
                return 0;
        }

        // Two potential results - very likely on collision.
        if (delta > epsilon) {
                int result_count = 0;
                const double sqrt_delta = sqrt(delta);

                const double t1 = (-b - sqrt_delta) / (2 * a);
                if (t1 >= 0 && t1 <= 1) {
                        ++result_count;
                        result.push_back({ x1 * t1 + x2 * (1 - t1),
                                           y1 * t1 + y2 * (1 - t1) });
                }

                const double t2 = (-b + sqrt_delta) / (2 * a);
                if (t2 >= 0 && t2 <= 1) {
                        ++result_count;
                        result.push_back({ x1 * t2 + x2 * (1 - t2),
                                           y1 * t2 + y2 * (1 - t2) });
                }

                return result_count;
        }

        // One collision - rare.
        const double t = (-b) / (2 * a);
        if (t < 0 || t > 1)
                return 0;

        result.push_back({ x1 * t + x2 * (1 - t),
                           y1 * t + y2 * (1 - t) });
        return 1;
}

int collide_circle_circle_impl(
                double x1, double y1, double r1,
                double x2, double y2, double r2,
                std::vector<efwk::point>& result)
{
        // 0.5 means half of pixel.
        static const double epsilon = 0.5;

#define SQR(X) ((X) * (X))

        // Precomputation.
        const double d = sqrt(SQR(x2 - x1) + SQR(y2 - y1));

        // Short circuit.
        // --------------

        // Too far.
        if (d > (r1 + r2)) {
                return 0;
        }

        // Too small and too inside.
        if (d < std::abs(r1 - r2)) {
                return 0;
        }

        // The solution.
        // -------------

        // The distance from p1 to pm - the middle of the intersection.
        double a = (SQR(r1) - SQR(r2) + SQR(d)) / (2 * d);

        // The coordinates of the point in the middle of the intersection.
        double xm = x1 + (a / d) * (x2 - x1);
        double ym = y1 + (a / d) * (y2 - y1);

        if (std::abs(d - (r1 + r2)) < epsilon) {
                result.push_back({ xm, ym });
                return 1;
        }

        // The distance from pm to intersection points.
        double h = sqrt(SQR(r1) + SQR(r2) / 2 * d - SQR(r1) / 2 * d - d / 2);

#undef SQR

        // The result points.
        result.push_back({ xm + h * (y2 - y1) / d, ym - h * (x2 - x1) / d });
        result.push_back({ xm - h * (y2 - y1) / d, ym + h * (x2 - x1) / d });
        return 2;
}

}

namespace efwk
{

// Typed geometry analysis and processing.
// =======================================

inline void transform_point(const orientation& ori, point& p)
{
        // Synthetically interpolate the orientation.
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        double phi = ori.interpolate_rot(0);

        // Rotate.
        p.x = p.x * cos(phi) - p.y * sin(phi);
        p.y = p.y * cos(phi) + p.x * sin(phi);

        // Translate.
        p.x += x;
        p.y += y;
}

std::vector<point> compute_square_points(const square& sqr)
{
        const double hs = sqr.side / 2.0;
        return {
                { -hs, -hs },
                {  hs, -hs },
                {  hs,  hs },
                { -hs,  hs }
        };
}

std::vector<segment> compute_square_segments(const square& sqr)
{
        const std::vector<point> pts = compute_square_points(sqr);
        return {
                { pts[0], pts[1] },
                { pts[1], pts[2] },
                { pts[2], pts[3] },
                { pts[3], pts[0] }
        };
}

// Specifficly typed collisions:
// =============================

// Segment to ... collisions:
// --------------------------

int collide_segment_segment(const segment& a, const orientation& a_ori,
                            const segment& b, const orientation& b_ori,
                            std::vector<point>& result)
{
        point aa = a.a;
        point ab = a.b;
        transform_point(a_ori, aa);
        transform_point(a_ori, ab);

        point ba = b.a;
        point bb = b.b;
        transform_point(b_ori, ba);
        transform_point(b_ori, bb);

        return collide_segment_segment_impl(
                        aa.x, aa.y, ab.x, ab.y,
                        ba.x, ba.y, bb.x, bb.y,
                        result);
}

int collide_segment_polygon(const segment& a, const orientation& a_ori,
                            const polygon& b, const orientation& b_ori,
                            std::vector<point>& result)
{
        int count = 0;

        point aa = a.a;
        point ab = a.b;
        transform_point(a_ori, aa);
        transform_point(a_ori, ab);

        for (const auto& b_seg : b.segs) {
                point ba = b_seg.a;
                point bb = b_seg.b;
                transform_point(b_ori, ba);
                transform_point(b_ori, bb);

                count += collide_segment_segment_impl(
                        aa.x, aa.y, ab.x, ab.y,
                        ba.x, ba.y, bb.x, bb.y,
                        result);
        }

        return count;
}

int collide_segment_square(const segment& a, const orientation& a_ori,
                           const square& b, const orientation& b_ori,
                           std::vector<point>& result)
{
        int count = 0;

        point aa = a.a;
        point ab = a.b;
        transform_point(a_ori, aa);
        transform_point(a_ori, ab);

        std::vector<segment> sqr_segs = compute_square_segments(b);
        for (const segment& b_seg : sqr_segs) {
                point ba = b_seg.a;
                point bb = b_seg.b;
                transform_point(b_ori, ba);
                transform_point(b_ori, bb);

                count += collide_segment_segment_impl(
                                aa.x, aa.y, ab.x, ab.y,
                                ba.x, ba.y, bb.x, bb.y,
                                result);
        }

        return count;
}

int collide_segment_circle(const segment& a, const orientation& a_ori,
                           const circle& b, const orientation& b_ori,
                           std::vector<point>& result)
{
        point aa = a.a;
        point ab = a.b;
        transform_point(a_ori, aa);
        transform_point(a_ori, ab);

        double x, y;
        std::tie(x, y) = b_ori.interpolate_loc(0);

        return collide_segment_circle_impl(
                        aa.x, aa.y, ab.x, ab.y,
                        x, y, b.radius,
                        result);
}

// Polygon to ... collisions:
// --------------------------

int collide_polygon_polygon(const polygon& a, const orientation& a_ori,
                            const polygon& b, const orientation& b_ori,
                            std::vector<point>& result)
{
        int count = 0;

        // Transform the points from both of the polygons.
        std::vector<point> transformed_a;
        for (const auto& a_seg : a.segs) {
                point aa = a_seg.a;
                transform_point(a_ori, aa);
                transformed_a.push_back(aa);

                point ab = a_seg.b;
                transform_point(a_ori, ab);
                transformed_a.push_back(ab);
        }

        std::vector<point> transformed_b;
        for (const auto& b_seg : b.segs) {
                point ba = b_seg.a;
                transform_point(b_ori, ba);
                transformed_b.push_back(ba);

                point bb = b_seg.b;
                transform_point(b_ori, bb);
                transformed_b.push_back(bb);
        }

        // Check for collisions each with each.
        unsigned i = 0;
        while (i < transformed_a.size()) {
                const point& aa = transformed_a[i];
                const point& ab = transformed_a[i + 1];
                unsigned j = 0;
                while (j < transformed_b.size()) {
                        const point& ba = transformed_b[i];
                        const point& bb = transformed_b[i + 1];

                        count += collide_segment_segment_impl(
                                aa.x, aa.y, ab.x, ab.y,
                                ba.x, ba.y, bb.x, bb.y,
                                result);

                        j += 2;
                }
                i += 2;
        }

        return count;
}

int collide_polygon_square(const polygon& a, const orientation& a_ori,
                           const square& b, const orientation& b_ori,
                           std::vector<point>& result)
{
        int count = 0;

        // Transform the points from both of the polygons.
        std::vector<point> transformed_a;
        for (const auto& a_seg : a.segs) {
                point aa = a_seg.a;
                transform_point(a_ori, aa);
                transformed_a.push_back(aa);

                point ab = a_seg.b;
                transform_point(a_ori, ab);
                transformed_a.push_back(ab);
        }

        std::vector<segment> b_segs = compute_square_segments(b);
        std::vector<point> transformed_b;
        for (const auto& b_seg : b_segs) {
                point ba = b_seg.a;
                transform_point(b_ori, ba);
                transformed_b.push_back(ba);

                point bb = b_seg.b;
                transform_point(b_ori, bb);
                transformed_b.push_back(bb);
        }

        // Check for collisions each with each.
        unsigned i = 0;
        while (i < transformed_a.size()) {
                const point& aa = transformed_a[i];
                const point& ab = transformed_a[i + 1];
                unsigned j = 0;
                while (j < transformed_b.size()) {
                        const point& ba = transformed_b[i];
                        const point& bb = transformed_b[i + 1];

                        count += collide_segment_segment_impl(
                                aa.x, aa.y, ab.x, ab.y,
                                ba.x, ba.y, bb.x, bb.y,
                                result);

                        j += 2;
                }
                i += 2;
        }

        return count;
}

int collide_polygon_circle(const polygon& a, const orientation& a_ori,
                           const circle& b, const orientation& b_ori,
                           std::vector<point>& result)
{
        int count = 0;

        double x, y;
        std::tie(x, y) = b_ori.interpolate_loc(0);

        for (const auto& a_seg : a.segs) {
                point aa = a_seg.a;
                point ab = a_seg.b;
                transform_point(a_ori, aa);
                transform_point(a_ori, ab);

                count += collide_segment_circle_impl(
                        aa.x, aa.y, ab.x, ab.y,
                        x, y, b.radius,
                        result);
        }

        return count;
}

// Square to ... collisions:
// -------------------------

int collide_square_square(const square& a, const orientation& a_ori,
                          const square& b, const orientation& b_ori,
                          std::vector<point>& result)
{
        int count = 0;

        // Transform the points from both of the polygons.
        std::vector<segment> a_segs = compute_square_segments(a);
        std::vector<point> transformed_a;
        for (const auto& a_seg : a_segs) {
                point aa = a_seg.a;
                transform_point(a_ori, aa);
                transformed_a.push_back(aa);

                point ab = a_seg.b;
                transform_point(a_ori, ab);
                transformed_a.push_back(ab);
        }

        std::vector<segment> b_segs = compute_square_segments(b);
        std::vector<point> transformed_b;
        for (const auto& b_seg : b_segs) {
                point ba = b_seg.a;
                transform_point(b_ori, ba);
                transformed_b.push_back(ba);

                point bb = b_seg.b;
                transform_point(b_ori, bb);
                transformed_b.push_back(bb);
        }

        // Check for collisions each with each.
        unsigned i = 0;
        while (i < transformed_a.size()) {
                const point& aa = transformed_a[i];
                const point& ab = transformed_a[i + 1];
                unsigned j = 0;
                while (j < transformed_b.size()) {
                        const point& ba = transformed_b[i];
                        const point& bb = transformed_b[i + 1];

                        count += collide_segment_segment_impl(
                                aa.x, aa.y, ab.x, ab.y,
                                ba.x, ba.y, bb.x, bb.y,
                                result);

                        j += 2;
                }
                i += 2;
        }

        return count;
}

int collide_square_circle(const square& a, const orientation& a_ori,
                          const circle& b, const orientation& b_ori,
                          std::vector<point>& result)
{
        int count = 0;

        double x, y;
        std::tie(x, y) = b_ori.interpolate_loc(0);

        std::vector<segment> a_segs = compute_square_segments(a);
        for (const auto& a_seg : a_segs) {
                point aa = a_seg.a;
                point ab = a_seg.b;
                transform_point(a_ori, aa);
                transform_point(a_ori, ab);

                count += collide_segment_circle_impl(
                        aa.x, aa.y, ab.x, ab.y,
                        x, y, b.radius,
                        result);
        }

        return count;
}

// Circle to ... collisions:
// -------------------------

int collide_circle_circle(const circle& a, const orientation& a_ori,
                          const circle& b, const orientation& b_ori,
                          std::vector<point>& result)
{

        double x1, y1;
        std::tie(x1, y1) = a_ori.interpolate_loc(0);

        double x2, y2;
        std::tie(x2, y2) = a_ori.interpolate_loc(0);

        return collide_circle_circle_impl(
                        x1, y1, a.radius,
                        x2, y2, b.radius,
                        result);
}

// Dispatching implementations.
// ============================

// Behold the double dispatch hell.
// It would be nice to somehow automatize this.

int segment::collide_with(const orientation& this_ori,
                          const shape& other, const orientation& other_ori,
                          std::vector<point>& result) const
{
        return other.collide_with_segment(other_ori, *this, this_ori, result);
}

int segment::collide_with_segment(const orientation& this_ori,
                                  const segment& other, const orientation& other_ori,
                                  std::vector<point>& result) const
{
        return collide_segment_segment(other, other_ori, *this, this_ori, result);
}

int segment::collide_with_polygon(const orientation& this_ori,
                                  const polygon& other, const orientation& other_ori,
                                  std::vector<point>& result) const
{
        return collide_segment_polygon(*this, this_ori, other, other_ori, result);
}

int segment::collide_with_square(const orientation& this_ori,
                                 const square& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_segment_square(*this, this_ori, other, other_ori, result);
}

int segment::collide_with_circle(const orientation& this_ori,
                                 const circle& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_segment_circle(*this, this_ori, other, other_ori, result);
}

int polygon::collide_with(const orientation& this_ori,
                          const shape& other, const orientation& other_ori,
                          std::vector<point>& result) const
{
        return other.collide_with_polygon(other_ori, *this, this_ori, result);
}

int polygon::collide_with_segment(const orientation& this_ori,
                                  const segment& other, const orientation& other_ori,
                                  std::vector<point>& result) const
{
        return collide_segment_polygon(other, other_ori, *this, this_ori, result);
}

int polygon::collide_with_polygon(const orientation& this_ori,
                                  const polygon& other, const orientation& other_ori,
                                  std::vector<point>& result) const
{
        return collide_polygon_polygon(other, other_ori, *this, this_ori, result);
}

int polygon::collide_with_square(const orientation& this_ori,
                                 const square& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_polygon_square(*this, this_ori, other, other_ori, result);
}

int polygon::collide_with_circle(const orientation& this_ori,
                                 const circle& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_polygon_circle(*this, this_ori, other, other_ori, result);
}

int square::collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const
{
        return other.collide_with_square(other_ori, *this, this_ori, result);
}

int square::collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_segment_square(other, other_ori, *this, this_ori, result);
}

int square::collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_polygon_square(other, other_ori, *this, this_ori, result);
}

int square::collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const
{
        return collide_square_square(other, other_ori, *this, this_ori, result);
}

int square::collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const
{
        return collide_square_circle(*this, this_ori, other, other_ori, result);
}

int circle::collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const
{
        return other.collide_with_circle(other_ori, *this, this_ori, result);
}

int circle::collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_segment_circle(other, other_ori, *this, this_ori, result);
}

int circle::collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const
{
        return collide_polygon_circle(other, other_ori, *this, this_ori, result);
}

int circle::collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const
{
        return collide_square_circle(other, other_ori, *this, this_ori, result);
}

int circle::collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const
{
        return collide_circle_circle(other, other_ori, *this, this_ori, result);
}

// Main dispatch point.
// --------------------

int collide(const shape& a, const orientation& a_ori,
            const shape& b, const orientation& b_ori,
            std::vector<point>& result)
{
        return a.collide_with(a_ori, b, b_ori, result);
}

}
