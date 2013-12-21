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

#define MAX_SEGS_IN_POLY 10

#include <vector>

#include "../sfinae.h"

namespace efwk
{

struct point
{
        double x, y;
};

struct shape_segment
{
        point a;
        point b;
};

SFINAE__DECLARE_HAS_MEMBER(HasSegmentShape, shape_segment, shp);

struct shape_polygon
{
        std::array<shape_segment, MAX_SEGS_IN_POLY> segs;
        int num_segs;
};

SFINAE__DECLARE_HAS_MEMBER(HasPolygonShape, shape_polygon, shp);

struct shape_square
{
        double side;
};

SFINAE__DECLARE_HAS_MEMBER(HasSquareShape, shape_square, shp);

struct shape_circle
{
        double radius;
};

SFINAE__DECLARE_HAS_MEMBER(HasCircleShape, shape_circle, shp);

template <class T>
using HasShape = TmpAny<HasSegmentShape<T>,
                        HasPolygonShape<T>,
                        HasSquareShape<T>,
                        HasCircleShape<T>>;


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

}

#endif
