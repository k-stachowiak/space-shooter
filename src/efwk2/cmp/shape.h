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

#ifndef SHAPE_H
#define SHAPE_H

static const int MAX_SEGS_IN_POLY = 10;

#include <cassert>
#include <vector>

#include "common.h"
#include "orientation.h"
#include "../tmp/sfinae.h"
#include "../tmp/traits.h"

namespace efwk
{

// Fundamental shape types.
// ========================

// Segment.
// --------

struct shape_segment
{
        point a;
        point b;
};

SFINAE__DECLARE_HAS_MEMBER(HasSegmentShape, shape_segment, shp);

// Polygon.
// --------

struct shape_polygon
{
        std::array<shape_segment, MAX_SEGS_IN_POLY> segs;
        int num_segs;
};

template <class Func>
void for_each_segment(const shape_polygon& poly, Func func)
{
        std::for_each(begin(poly.segs), begin(poly.segs) + poly.num_segs, func);
}

SFINAE__DECLARE_HAS_MEMBER(HasPolygonShape, shape_polygon, shp);

// Square.
// -------

struct shape_square
{
        double side;
};

template <class Func>
void for_each_segment(const shape_square& sqr, Func func)
{
        const double hs = sqr.side / 2.0;
        const point points[4] = {
                { -hs, -hs },
                {  hs, -hs },
                {  hs,  hs },
                { -hs,  hs }
        };

        func(shape_segment { points[0], points[1] });
        func(shape_segment { points[1], points[2] });
        func(shape_segment { points[2], points[3] });
        func(shape_segment { points[3], points[0] });
}

SFINAE__DECLARE_HAS_MEMBER(HasSquareShape, shape_square, shp);

// Circle.
// -------

struct shape_circle
{
        double radius;
        shape_circle(double new_radius) : radius(new_radius) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasCircleShape, shape_circle, shp);

// Compound shape.
// ---------------

struct shape_compound_base {};

template <class... Shapes>
struct shape_compound : public shape_compound_base
{
        std::tuple<std::pair<Shapes, orientation>...> impl;
        shape_compound(const std::pair<Shapes, orientation>&... new_impl) :
                impl(new_impl...)
        {}
};

template <class Func, int ShapeIndex, class... Shapes>
struct cpd_shp_functor
{
        void operator()(const shape_compound<Shapes...>& cpd, Func func)
        {
                const auto& pr = std::get<ShapeIndex>(cpd.impl);
                const auto& shp = pr.first;
                const orientation& ori = pr.second;
                func(shp, ori);

                cpd_shp_functor<Func, ShapeIndex - 1, Shapes...> ftor;
                ftor(cpd, func);
        }
};

template <class Func, class... Shapes>
struct cpd_shp_functor<Func, -1, Shapes...>
{
        void operator()(const shape_compound<Shapes...>&, Func) {}
};

template <class Func, class... Shapes>
void for_each_shape(const shape_compound<Shapes...>& cpd, Func func)
{
        // NOTE: this iterates backwards.
        cpd_shp_functor<Func, sizeof...(Shapes) - 1, Shapes...> fctor;
        fctor(cpd, func);
}

SFINAE__DECLARE_HAS_MEMBER(HasCompoundShape, shape_compound_base, shp);

template <class T>
using HasShape = TmpAny<HasSegmentShape<T>,
                        HasPolygonShape<T>,
                        HasSquareShape<T>,
                        HasCircleShape<T>,
                        HasCompoundShape<T>>;

// Shape transformations.
// ======================

inline
point trans(const point& in, const orientation& ori)
{
        const double phi = ori.interpolate_rot(0);
        double x, y;
        std::tie(x, y) = ori.interpolate_loc(0);

        // Rotate.
        double out_x = in.x * cos(phi) - in.y * sin(phi);
        double out_y = in.y * cos(phi) + in.x * sin(phi);

        // Translate.
        out_x += x;
        out_y += y;

        return { out_x, out_y };
}

inline
shape_segment trans(const shape_segment& in, const orientation& ori)
{
        return shape_segment { trans(in.a, ori), trans(in.b, ori) };
}

// Shape decomposition.
// ====================

inline
std::vector<efwk::point> points(const efwk::shape_square& in)
{
        const double hs = in.side / 2.0;
        return {
                { -hs, -hs },
                {  hs, -hs },
                {  hs,  hs },
                { -hs,  hs }
        };
}

inline
std::vector<efwk::shape_segment> segments(const efwk::shape_square& sqr)
{
        const std::vector<efwk::point> pts = points(sqr);
        return {
                { pts[0], pts[1] },
                { pts[1], pts[2] },
                { pts[2], pts[3] },
                { pts[3], pts[0] }
        };
}

// Generating a radom point within the shapes.
// ===========================================

// TODO: copy the implementation.

}

#endif
