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

template<class Iter>
shape_polygon make_polygon(Iter first, Iter last)
{
        shape_polygon result;

        const int size = std::distance(first, last);

        assert(size <= MAX_SEGS_IN_POLY);
        std::copy(first, last, &(result.segs.front()));
        result.num_segs = size;

        return result;
}

SFINAE__DECLARE_HAS_MEMBER(HasPolygonShape, shape_polygon, shp);

// Square.
// -------

struct shape_square
{
        double side;
};

SFINAE__DECLARE_HAS_MEMBER(HasSquareShape, shape_square, shp);

// Circle.
// -------

struct shape_circle
{
        double radius;
        shape_circle(double new_radius) : radius(new_radius) {}
};

SFINAE__DECLARE_HAS_MEMBER(HasCircleShape, shape_circle, shp);

template <class T>
using HasShape = TmpAny<HasSegmentShape<T>,
                        HasPolygonShape<T>,
                        HasSquareShape<T>,
                        HasCircleShape<T>>;

// Shape transformations.
// ======================

inline
point trans(const point& in, double dx, double dy, double phi)
{
        // Rotate.
        double out_x = in.x * cos(phi) - in.y * sin(phi);
        double out_y = in.y * cos(phi) + in.x * sin(phi);

        // Translate.
        out_x += dx;
        out_y += dy;

        return { out_x, out_y };
}

inline
shape_segment trans(const shape_segment& in,
                    double x, double y, double phi)
{
        return shape_segment {
                trans(in.a, x, y, phi),
                trans(in.b, x, y, phi) };
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
