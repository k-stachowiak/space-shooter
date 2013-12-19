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

#include "../cmp/shape.h"
#include "../cmp/orientation.h"

namespace
{

point trans(const point& in, const orientation& ori)
{
        // Rotate.
        const double phi = ori.interpolate_rot(0);
        double out_x = i.x * cos(phi) - in.y * sin(phi);
        double out_y = i.y * cos(phi) + in.x * sin(phi);

        // Translate.
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        out_x += x;
        out_y += y;

        return { out_x, out_y };
}

shape_segment trans(const shape_segment& in, const orientation& ori)
{
        shape_segment { trans(in.a, ori), trans(in.b, ori) };
}

std::vector<point> points(const shape_square& in)
{
        const double hs = in.side / 2.0;
        return {
                { -hs, -hs },
                {  hs, -hs },
                {  hs,  hs },
                { -hs,  hs }
        };
}

std::vector<segment> segments(const square& sqr)
{
        const std::vector<point> pts = compute_square_points(sqr);
        return {
                { pts[0], pts[1] },
                { pts[1], pts[2] },
                { pts[2], pts[3] },
                { pts[3], pts[0] }
        };
}

}

namespace efwk
{

// Simple collision implementations.
// ---------------------------------

template <class Iter>
int collide_impl(const shape_segment& seg1,
                 const shape_segment& seg2,
                 Iter& out)
{
        // Decompose input.
        const double& ax1 = seg1.a.x;
        const double& ay1 = seg1.a.y;
        const double& bx1 = seg1.b.x;
        const double& by1 = seg1.b.y;
        const double& ax2 = seg2.a.x;
        const double& ay2 = seg2.a.y;
        const double& bx2 = seg2.b.x;
        const double& by2 = seg2.b.y;

        // Perform computations.
        const double sx1 = bx1 - ax1;
        const double sy1 = by1 - ay1;
        const double sx2 = bx2 - ax2;
        const double sy2 = by2 - ay2;

        const float s = (-sy1 * (ax1 - ax2) + sx1 * (ay1 - ay2)) / (-sx2 * sy1 + sx1 * sy2);
        const float t = (sx2 * (ay1 - ay2) - sy2 * (ax1 - ax2)) / (-sx2 * sy1 + sx1 * sy2);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
                *(out++) = { seg1.a.x + (t * sx1), seg1.a.y + (t * sy1) };
                return 1;
        } else {
                return 0;
        }
}

template <class Iter>
int collide_impl(const shape_segment& seg,
                 const shape_circle& cir,
                 const orientation& cir_ori,
                 Iter& out)
{
        // 0.5 means half of pixel.
        static const double epsilon = 0.5;

        // Decmopose input.
        const double& x1 = seg.a.x;
        const double& y1 = seg.a.y;
        const double& x2 = seg.b.x;
        const double& y2 = seg.b.y;
        const double& r = cir.radius;
        double xc, yc;
        std::tie(xc, yc) = cir_ori.interpolate(0);

        // Perform computations.

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
                        *(out++) = { x1 * t1 + x2 * (1 - t1),
                                     y1 * t1 + y2 * (1 - t1) };
                }

                const double t2 = (-b + sqrt_delta) / (2 * a);
                if (t2 >= 0 && t2 <= 1) {
                        ++result_count;
                        *(out++) = { x1 * t2 + x2 * (1 - t2),
                                     y1 * t2 + y2 * (1 - t2) };
                }

                return result_count;
        }

        // One collision - rare.
        const double t = (-b) / (2 * a);
        if (t < 0 || t > 1)
                return 0;

        *(out++) = { x1 * t + x2 * (1 - t),
                     y1 * t + y2 * (1 - t) };
        return 1;
}

template <class Iter>
int collide_impl(const shape_circle& cir1, const orientation& ori1,
                 const shape_circle& cir2, const orientation& ori2,
                 Iter& out)
{
        // 0.5 means half of pixel.
        static const double epsilon = 0.5;

        // Decompose the input.
        const double& r1 = cir1.radius;
        const double& r2 = cir2.radius;

        double x1, y1, x2, y2;
        std::tie(x1, y1) = ori1.interpolate(0);
        std::tie(x2, y2) = ori2.interpolate(0);

        // Perform the computation.

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
                *(out++) = { xm, ym };
                return 1;
        }

        // The distance from pm to intersection points.
        double h = sqrt(SQR(r1) + SQR(r2) / 2 * d - SQR(r1) / 2 * d - d / 2);

#undef SQR

        // The result points.
        *(out++) = { xm + h * (y2 - y1) / d, ym - h * (x2 - x1) / d };
        *(out++) = { xm - h * (y2 - y1) / d, ym + h * (x2 - x1) / d };
        return 2;
}

// Complex collision implementation.
// ---------------------------------

int collide_impl(const shape_segment& seg,
                 const shape_polygon& poly,
                 Iter& out)
{
        const auto first = begin(poly.segs);
        const auto last = begin(poly.segs) + poly.num_segs;
        int count = 0;
        for (auto s = first; s != last; ++s) {
                count += collide_segment_segment(seg, *s, out);
        }
        return count;
}

int collide_impl(const shape_segment& seg,
                 const shape_square& sqr,
                 const orientation& sqr_ori,
                 Iter& out)
{
        int count = 0;
        for (const auto& s : segments(sqr)) {
                auto transformed = trans(s, sqr_ori);
                count += collide_segment_segment(seg, transformed, out);
        }
        return count;
}

// Dispatch implementation.
// ========================

template <class T>
using IsCollidable = HasShape<T>;

template <class Entity1, class Entity2>
typename std::enable_if<IsCollidable<Entity1>::value &&
                        IsCollidable<Entity2>::value, int>::type
check_collisions(Entity1& ent1, Entity2& ent2)
{
        long id1 = ent1.id;
        const auto& shp1 = ent1.shp;
        const auto& ori1 = ent1.ori;

        long id2 = ent2.id;
        const auto& shp2 = ent2.shp;
        const auto& ori2 = ent2.ori;

        return collide_impl(shp1, ori1, shp2, ori2);
}

template <class Entity1, class Entity2>
typename std::enable_if<!IsCollidable<Entity1>::value ||
                        !IsCollidable<Entity2>::value, int>::type
check_collisions(Entity1&, Entity2&) {}

// Operation: Collide collections.
// -------------------------------
//
// Performs collection-level collision checking. This has two cases:
//
// (1) When a single collection is provided as an argument, the "self" checks
//     are performed, i.e. only the pairs of the elements within the collection
//     are tested.
// (2) When a pair of collections is provided, then each element from the first
//     collection is tested in pair with each element from the second collecrion.
// (3) None of the elements is tested with itself.
// (4) Only one half of the interaction matrix is checked, i.e. if A element was
//     compared with B element, then no B with A check will be performed.

template <class Coll>
int collide_coll(Coll& coll)
{
        int count = 0;
        for (auto i = begin(coll); i != end(coll); ++i)
                for (auto j = (i + 1); j != end(coll); ++j)
                        count += check_collisions(*i, *j);
        return count;
}

template <class Coll1, class Coll2>
int collide_coll(Coll1& coll1, Coll2& coll2)
{
        int count = 0;
        for (const auto& i : coll1)
                for (const auto& j : coll2)
                        count += check_collisions(i, j);
        return count;
}

// Operation: Collide first with rest.
// -----------------------------------
//
// Takes a sequence of collections: c0, c1, c2, ... and performs the collisions check
// for all the collection pairs: (c0 c1), (c0 c2), ... .
//
// (1) The call to the "collide collections" handles the collision between the
//     first and the second argument.
// (2) The recursive call drops the second argument and continues the operation
//     for the further arguments.
// (3) When a single argument is left, nothing is performed. NOTE: this operation
//     doesn't perform the checks for the collisions between the first argument
//     and self.
// (4) The results of all the checks are summed and returned. NOTE: in the
//     recursion terminating case, no operation is performed and therefore 0 is
//     returned.

template <class Coll>
int collide_first_rest(Coll& coll)
{
        return 0;
}

template <class Coll1, class Coll2, class... Rest>
int collide_first_rest(Coll1& coll1, Coll2& coll2, Rest... rest)
{
        return collide_coll(coll1, coll2) +
               collide_first_rest(coll1, rest...);
}

// Operation: Collide all.
// -----------------------
//
// Takes a sequence of collections for a carthesian product of the union of
// the collections (only the upper half of the matrix) and performs the collisions
// check for all such pairs of collections.
//
// (1) The call to "collide collection" handles the first collection's test with
//     itself.
// (2) The call to "collide first wiht rest" handles the collisions check between
//     the first argument and each of the following ones
// (3) The recursive call to "collide all" handles performing (1) for all of the
//     following arguments...
// (4) ...exccept for the case when only one argument is left in the recursion.
//     then only the collisions within the single collection are handled.
// (5) The numbers of the collisions from all the checks are summed and returned.

template <class Coll>
int collide_all(Coll& coll)
{
        return collide_coll(coll, coll);
}

template <class Coll1, class Coll2, class... Rest>
int collide_all(Coll1& first, Coll2& second, Rest... rest)
{
        return collide_coll(first) +
               collide_first_rest(first, second, rest...) +
               collide_all(second, rest...);
}

}
