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

#include "../../misc/logger.h"

#include "../cmp/shape.h"
#include "../cmp/orientation.h"
#include "../cmp/coll_traits.h"
#include "../cmp/coll_queue.h"
#include "../tmp/traits.h"

namespace efwk
{

// Simple collision implementations.
// ---------------------------------

// Segment - Segment
template <class PointIt>
bool collide_impl(const shape_segment& seg1, const orientation& seg1_ori,
                  const shape_segment& seg2, const orientation& seg2_ori,
                  PointIt& point_it)
{
        const shape_segment trans_seg1 = trans(seg1, seg1_ori);
        const shape_segment trans_seg2 = trans(seg2, seg2_ori);

        // Decompose input.
        const double& ax1 = trans_seg1.a.x;
        const double& ay1 = trans_seg1.a.y;
        const double& bx1 = trans_seg1.b.x;
        const double& by1 = trans_seg1.b.y;
        const double& ax2 = trans_seg2.a.x;
        const double& ay2 = trans_seg2.a.y;
        const double& bx2 = trans_seg2.b.x;
        const double& by2 = trans_seg2.b.y;

        // Perform computations.
        const double sx1 = bx1 - ax1;
        const double sy1 = by1 - ay1;
        const double sx2 = bx2 - ax2;
        const double sy2 = by2 - ay2;

        const float s = (-sy1 * (ax1 - ax2) + sx1 * (ay1 - ay2)) / (-sx2 * sy1 + sx1 * sy2);
        const float t = (sx2 * (ay1 - ay2) - sy2 * (ax1 - ax2)) / (-sx2 * sy1 + sx1 * sy2);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
                *(point_it++) = { seg1.a.x + (t * sx1), seg1.a.y + (t * sy1) };
                return true;
        }

        return false;
}

// Segment - Circle
template <class PointIt>
bool collide_impl(const shape_segment& seg, const orientation& seg_ori,
                  const shape_circle& cir, const orientation& cir_ori,
                  PointIt& point_it)
{
        const shape_segment trans_seg = trans(seg, seg_ori);

        // Decmopose input.
        const double& x1 = trans_seg.a.x;
        const double& y1 = trans_seg.a.y;
        const double& x2 = trans_seg.b.x;
        const double& y2 = trans_seg.b.y;
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
                return false;
        }

        // Two potential results - very likely on collision.
        if (delta > EPSILON) {
                const double sqrt_delta = sqrt(delta);

                const double t1 = (-b - sqrt_delta) / (2 * a);
                if (t1 >= 0 && t1 <= 1) {
                        *(point_it++) = { x1 * t1 + x2 * (1 - t1),
                                     y1 * t1 + y2 * (1 - t1) };
                }

                const double t2 = (-b + sqrt_delta) / (2 * a);
                if (t2 >= 0 && t2 <= 1) {
                        *(point_it++) = { x1 * t2 + x2 * (1 - t2),
                                     y1 * t2 + y2 * (1 - t2) };
                }

                return true;
        }

        // One collision - rare.
        const double t = (-b) / (2 * a);
        if (t < 0 || t > 1)
                return false;

        *(point_it++) = { x1 * t + x2 * (1 - t),
                     y1 * t + y2 * (1 - t) };
        return true;
}

template <class PointIt>
bool collide_impl(const shape_circle& cir, const orientation& cir_ori,
                  const shape_segment& seg, const orientation& seg_ori,
                  PointIt& point_it)
{
        return collide_impl(seg, seg_ori, cir, cir_ori, point_it);
}

// Circle - Circle
template <class PointIt>
bool collide_impl(const shape_circle& cir1, const orientation& ori1,
                  const shape_circle& cir2, const orientation& ori2,
                  PointIt& point_it)
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
                return false;
        }

        // Too small and too inside.
        // Note: True returned, but no collision points added.
        if (d < std::abs(r1 - r2)) {
                return true;
        }

        // The solution.
        // -------------

        // The distance from p1 to pm - the middle of the intersection.
        const double a = (SQR(r1) - SQR(r2) + SQR(d)) / (2 * d);

        // The coordinates of the point in the middle of the intersection.
        const double xm = x1 + (a / d) * (x2 - x1);
        const double ym = y1 + (a / d) * (y2 - y1);

        if (std::abs(d - (r1 + r2)) < EPSILON) {
                *(point_it++) = { xm, ym };
                return true;
        }

        // The distance from pm to intersection points.
        const double h = sqrt(SQR(r1) - SQR(a));

#undef SQR

        // The result points.
        *(point_it++) = { xm + h * (y2 - y1) / d, ym - h * (x2 - x1) / d };
        *(point_it++) = { xm - h * (y2 - y1) / d, ym + h * (x2 - x1) / d };
        return true;
}

// Complex collision implementation.
// =================================

// Collision functors.
// -------------------

template <class OtherShape, class PointIt>
struct seg_collide_func
{
        // In.
        const orientation& poly_ori;
        const OtherShape& other;
        const orientation& other_ori;

        // Out.
        PointIt& point_it;
        bool& result;

        void operator()(const shape_segment& seg)
        {
                result |= collide_impl(seg, poly_ori, other, other_ori, point_it);
        }
};

template <class OtherShape, class PointIt>
struct shp_collide_func
{
        // In.
        const orientation& cpd_ori;
        const OtherShape& other;
        const orientation& other_ori;

        // Out.
        PointIt& point_it;
        bool& result;

        template <class CurrentShape>
        void operator()(int,
                        const CurrentShape& current_shp,
                        const orientation& current_ori)
        {
                orientation composed_ori = compose(current_ori, cpd_ori);
                result |= collide_impl(current_shp, composed_ori,
                                       other, other_ori,
                                       point_it);
        };
};

template <class PointIt, class... OtherShapes>
struct shpcpd_collide_func
{
        // In.
        const orientation cpd_ori;
        const shape_compound<OtherShapes...> other;
        const orientation& other_ori;

        // Out.
        PointIt& point_it;
        bool& result;

        template <class CurrentShape>
        void operator()(int,
                        const CurrentShape& current_shp,
                        const orientation current_ori)
        {
                orientation composed_ori = compose(current_ori, cpd_ori);
                for_each_shape(other, shp_collide_func<CurrentShape, PointIt> {
                        other_ori, current_shp, composed_ori, point_it, result
                });
        }
};

// Collision implementations.
// --------------------------

// Square - whatever.

template <class OtherShape, class PointIt>
bool collide_impl(const shape_square& sqr, const orientation& sqr_ori,
                  const OtherShape& other, const orientation& other_ori,
                  PointIt& point_it)
{
        bool result = false;
        for_each_segment(sqr, seg_collide_func<OtherShape, PointIt> {
                sqr_ori, other, other_ori, point_it, result
        });
        return result;
}

template <class OtherShape, class PointIt>
bool collide_impl(const OtherShape& other, const orientation& other_ori,
                  const shape_square& sqr, const orientation& sqr_ori,
                  PointIt& point_it)
{
        return collide_impl(sqr, sqr_ori, other, other_ori, point_it);
}

// Polygon - whatever.

template <class OtherShape, class PointIt>
bool collide_impl(const shape_polygon& poly, const orientation& poly_ori,
                  const OtherShape& other, const orientation& other_ori,
                  PointIt& point_it)
{
        bool result = false;
        for_each_segment(poly, seg_collide_func<OtherShape, PointIt> {
                poly_ori, other, other_ori, point_it, result
        });
        return result;
}

template <class OtherShape, class PointIt>
bool collide_impl(const OtherShape& other, const orientation& other_ori,
                  const shape_polygon& poly, const orientation& poly_ori,
                  PointIt& point_it)
{
        return collide_impl(poly, poly_ori, other, other_ori, point_it);
}

// Compound - whatever.

template <class... CpdShapes, class OtherShape, class PointIt>
bool collide_impl(const shape_compound<CpdShapes...>& cpd, const orientation& cpd_ori,
                  const OtherShape& other, const orientation& other_ori,
                  PointIt& point_it)
{
        bool result = false;
        for_each_shape(cpd, shp_collide_func<OtherShape, PointIt> {
                cpd_ori, other, other_ori, point_it, result
        });
        return result;
}

template <class... CpdShapes, class OtherShape, class PointIt>
bool collide_impl(const OtherShape& other, const orientation& other_ori,
                  const shape_compound<CpdShapes...>& cpd, const orientation& cpd_ori,
                  PointIt& point_it)
{
        return collide_impl(cpd, cpd_ori, other, other_ori, point_it);
}

template <class... CpdShapes, class... OtherShapes, class PointIt>
bool collide_impl(const shape_compound<CpdShapes...>& cpd1, const orientation& cpd1_ori,
                  const shape_compound<OtherShapes...>& cpd2, const orientation& cpd2_ori,
                  PointIt& point_it)
{
        bool result = false;
        for_each_shape(cpd1, shpcpd_collide_func<PointIt, OtherShapes...> {
                cpd1_ori, cpd2, cpd2_ori, point_it, result
        });
        return result;
}

// Binary proxy - whatever.

template <class Shape1, class Shape2, class OtherShape, class PointIt>
bool collide_impl(const shape_bin_proxy<Shape1, Shape2>& bpx, const orientation& bpx_ori,
                  const OtherShape& other, const orientation other_ori,
                  PointIt& point_it)
{
        return (bpx.state == bp_state::first)
                ? collide_impl(bpx.shp1, bpx_ori, other, other_ori, point_it)
                : collide_impl(bpx.shp2, bpx_ori, other, other_ori, point_it);
}

template <class Shape1, class Shape2, class OtherShape, class PointIt>
bool collide_impl(const OtherShape& other, const orientation other_ori,
                  const shape_bin_proxy<Shape1, Shape2>& bpx, const orientation& bpx_ori,
                  PointIt& point_it)
{
        return collide_impl(bpx, bpx_ori, other, other_ori, point_it);
}

template <class Shape1, class Shape2, class Shape3, class Shape4, class PointIt>
bool collide_impl(const shape_bin_proxy<Shape1, Shape2>& bpx1, const orientation& bpx1_ori,
                  const shape_bin_proxy<Shape3, Shape4>& bpx2, const orientation& bpx2_ori,
                  PointIt& point_it)
{
        if (bpx1.state == bp_state::first)
                if (bpx2.state == bp_state::first)
                        return collide_impl(bpx1.shp1, bpx1_ori, bpx2.shp1, bpx2_ori, point_it);
                else
                        return collide_impl(bpx1.shp1, bpx1_ori, bpx2.shp2, bpx2_ori, point_it);
        else
                if (bpx2.state == bp_state::first)
                        return collide_impl(bpx1.shp2, bpx1_ori, bpx2.shp1, bpx2_ori, point_it);
                else
                        return collide_impl(bpx1.shp2, bpx1_ori, bpx2.shp2, bpx2_ori, point_it);
}

// Dispatch implementation.
// ========================

template <class T>
using IsCollidable = TmpAll<HasShape<T>,
                            HasCollisionTraits<T>,
                            HasCollisionQueue<T>>;

template <class Entity1, class Entity2>
typename std::enable_if<IsCollidable<Entity1>::value &&
                        IsCollidable<Entity2>::value, void>::type
check_collisions(Entity1& ent1, Entity2& ent2)
{
        // Determine the collision points.
        std::vector<point> points;
        auto inserter = std::back_inserter(points);
        const bool result = collide_impl(ent1.shp, ent1.ori, ent2.shp, ent2.ori, inserter);

        // Store the report.
        if (result) {
                ent1.cqueue.push({ ent2.id, ent2.score_id, ent2.type_id,
                                   ent2.ctraits.cteam,
                                   ent2.ctraits.cclass,
                                   ent2.ctraits.cdmg,
                                   ent2.ctraits.cpck,
                                   points });
                ent2.cqueue.push({ ent1.id, ent1.score_id, ent1.type_id,
                                   ent1.ctraits.cteam,
                                   ent1.ctraits.cclass,
                                   ent1.ctraits.cdmg,
                                   ent1.ctraits.cpck,
                                   points });
        }
}

template <class Entity1, class Entity2>
typename std::enable_if<!IsCollidable<Entity1>::value ||
                        !IsCollidable<Entity2>::value, void>::type
check_collisions(Entity1&, Entity2&) {}

}

#endif
