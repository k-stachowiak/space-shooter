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

#ifndef COLLISION_H
#define COLLISION_H

// 0.5 means half of pixel.
static const double EPSILON = 0.5;

#include "../cmp/shape.h"
#include "../cmp/orientation.h"

// TODO: implement the transformations wherever needed.
//       right now many of the orientation arguments are not used. Fortunately
//       the compiler shall point them out.

namespace efwk
{

// Orientation based overloads for the transformation operation.
// -------------------------------------------------------------

inline
point trans(const point& in, const orientation& ori)
{
        const double phi = ori.interpolate_rot(0);
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        return trans(in, x, y, phi);
}

inline
shape_segment trans(const shape_segment& in,
                    const orientation& ori)
{
        const double phi = ori.interpolate_rot(0);
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);
        return trans(in, x, y, phi);
}


// Simple collision implementations.
// ---------------------------------

template <class Iter>
int collide_impl(const shape_segment& seg1, const orientation& seg1_ori,
                 const shape_segment& seg2, const orientation& seg2_ori,
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
int collide_impl(const shape_segment& seg, const orientation& seg_ori,
                 const shape_circle& cir, const orientation& cir_ori,
                 Iter& out)
{
        // Decmopose input.
        const double& x1 = seg.a.x;
        const double& y1 = seg.a.y;
        const double& x2 = seg.b.x;
        const double& y2 = seg.b.y;
        const double& r = cir.radius;
        double xc, yc;
        std::tie(xc, yc) = cir_ori.interpolate_loc(0);

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
        if (delta < -EPSILON) {
                return 0;
        }

        // Two potential results - very likely on collision.
        if (delta > EPSILON) {
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
int collide_impl(const shape_circle& cir, const orientation& cir_ori,
                 const shape_segment& seg, const orientation& seg_ori,
                 Iter& out)
{
        return collide_impl(seg, seg_ori, cir, cir_ori, out);
}

template <class Iter>
int collide_impl(const shape_circle& cir1, const orientation& ori1,
                 const shape_circle& cir2, const orientation& ori2,
                 Iter& out)
{

        // Decompose the input.
        const double& r1 = cir1.radius;
        const double& r2 = cir2.radius;

        double x1, y1, x2, y2;
        std::tie(x1, y1) = ori1.interpolate_loc(0);
        std::tie(x2, y2) = ori2.interpolate_loc(0);

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

        if (std::abs(d - (r1 + r2)) < EPSILON) {
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

template <class Iter>
int collide_impl(const shape_segment& seg, const orientation& seg_ori,
                 const shape_polygon& poly, const orientation& poly_ori,
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

template <class Iter>
int collide_impl(const shape_polygon& poly, const orientation& poly_ori,
                 const shape_segment& seg, const orientation& seg_ori,
                 Iter& out)
{
        return collide_impl(seg, seg_ori, poly, poly_ori, out);
}

template <class Iter>
int collide_impl(const shape_segment& seg, const orientation& seg_ori,
                 const shape_square& sqr, const orientation& sqr_ori,
                 Iter& out)
{
        int count = 0;
        for (const auto& s : segments(sqr)) {
                auto transformed = trans(s, sqr_ori);
                count += collide_segment_segment(seg, transformed, out);
        }
        return count;
}

template <class Iter>
int collide_impl(const shape_square& sqr, const orientation& sqr_ori,
                 const shape_segment& seg, const orientation& seg_ori,
                 Iter& out)
{
        return collide_impl(seg, seg_ori, sqr, sqr_ori, out);
}

// Dispatch implementation.
// ========================

template <class T>
using IsCollidable = HasShape<T>;

template <class Out, class Entity1, class Entity2>
typename std::enable_if<IsCollidable<Entity1>::value &&
                        IsCollidable<Entity2>::value, int>::type
check_collisions(Entity1& ent1, Entity2& ent2, Out& out)
{
        std::cout << "check_collision" << std::endl;
        return collide_impl(ent1.shp, ent1.ori,
                            ent2.shp, ent2.ori,
                            out);
}

template <class Out, class Entity1, class Entity2>
typename std::enable_if<!IsCollidable<Entity1>::value ||
                        !IsCollidable<Entity2>::value, int>::type
check_collisions(Entity1&, Entity2&, Out&) { return 0; }

// Operation: Collide leaf.
// ------------------------
// Performs the low level dispatch of the collision checking between collections
// and single objects.

template <class Out, class Coll>
typename std::enable_if<IsCollection<Coll>::value, int>::type
collide_leaf(Out& out, Coll& coll)
{
        // Collection with itself.
        int count = 0;
        for (auto i = begin(coll); i != end(coll); ++i)
                for (auto j = (i + 1); j != end(coll); ++j)
                        count += check_collisions(*i, *j, out);
        return count;
}

template <class Out, class Ent>
typename std::enable_if<!IsCollection<Ent>::value, int>::type
collide_leaf(Out& out, Ent& ent)
{
        // Entity with itself.
        return 0;
}

template <class Out, class Coll1, class Coll2>
typename std::enable_if<IsCollection<Coll1>::value &&
                        IsCollection<Coll2>::value, int>::type
collide_leaf(Out& out, Coll1& coll1, Coll2& coll2)
{
        // Collection with a collection.
        int count = 0;
        for (const auto& i : coll1)
                for (const auto& j : coll2)
                        count += check_collisions(i, j, out);
        return count;
}

template <class Out, class Ent, class Coll>
typename std::enable_if<IsCollection<Coll>::value &&
                        !IsCollection<Ent>::value, int>::type
collide_leaf(Out& out, Ent& ent, Coll& coll)
{
        // Entity with a collection.
        int count = 0;
        for (const auto& j : coll)
                count += check_collisions(ent, j, out);
        return count;
}

template <class Out, class Ent, class Coll>
typename std::enable_if<IsCollection<Coll>::value &&
                        !IsCollection<Ent>::value, int>::type
collide_leaf(Out& out, Coll& coll, Ent& ent)
{
        return collide_leaf(out, ent, coll);
}

template <class Out, class Ent1, class Ent2>
typename std::enable_if<!IsCollection<Ent1>::value &&
                        !IsCollection<Ent2>::value, int>::type
collide_leaf(Out& out, Ent1& ent1, Ent2& ent2)
{
        // Entity with an entity.
        return check_collisions(ent1, ent2, out);
}

// Operation: Collide first with rest.
// -----------------------------------
// Takes a sequence of collections: c0, c1, c2, ... and performs the collisions check
// for all the collection pairs: (c0 c1), (c0 c2), ... .

template <class Out, class Coll>
int collide_first_rest(Out&, Coll&)
{
        return 0;
}

template <class Out, class Coll1, class Coll2, class... Rest>
int collide_first_rest(Out& out, Coll1& coll1, Coll2& coll2, Rest&... rest)
{
        return collide_leaf(out, coll1, coll2) +
               collide_first_rest(out, coll1, rest...);
}

// Operation: Collide all.
// -----------------------
// Takes a sequence of collections for a carthesian product of the union of
// the collections (only the upper half of the matrix) and performs the collisions
// check for all such pairs of collections.

template <class Out, class Coll>
int collide_all(Out& out, Coll& coll)
{
        return collide_leaf(out, coll);
}

template <class Out, class Coll1, class Coll2, class... Rest>
int collide_all(Out& out, Coll1& first, Coll2& second, Rest&... rest)
{
        return collide_leaf(out, first) +
               collide_first_rest(out, first, second, rest...) +
               collide_all(out, second, rest...);
}

}

#endif
