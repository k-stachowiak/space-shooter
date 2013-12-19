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

#include <vector>

#include "orientation.h"

namespace efwk
{

struct point
{
        double x, y;
};

enum class shape_type
{
        segment,
        polygon,
        square,
        circle
};

struct shape_segment
{
        point a;
        point b;
};

struct shape_polygon
{
        std::array<shape_segment, MAX_SEGS_IN_POLY> segs;
        int num_segs;
};

struct shape_square
{
        double side;
};

struct shape_circle
{
        double radius;
};

struct shape
{
        shape_type type;
        shape_segment segment;
        shape_polygon polygon;
        shape_square square;
        shape_circle circle;
};

inline shape_segment make_segment(double x1, double x2, double y1, double y2)
{
        return {{ x1, y1 }, { x2, y2 }};
}

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

inline shape_square make_square(double side)
{
        return { side };
}

inline shape_circle make_circle(double radius)
{
        return { radius };
}

}

#endif
