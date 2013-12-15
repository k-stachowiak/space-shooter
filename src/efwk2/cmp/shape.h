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

class segment;
class polygon;
class square;
class circle;

struct shape
{
        virtual int collide_with(const orientation& this_ori,
                        const shape& other, const orientation& other_ori,
                        std::vector<point>& result) const = 0;

        virtual int collide_with_segment(const orientation& this_ori,
                        const segment& other, const orientation& other_ori,
                        std::vector<point>& result) const = 0;

        virtual int collide_with_polygon(const orientation& this_ori,
                        const polygon& other, const orientation& other_ori,
                        std::vector<point>& result) const = 0;

        virtual int collide_with_square(const orientation& this_ori,
                        const square& other, const orientation& other_ori,
                        std::vector<point>& result) const = 0;

        virtual int collide_with_circle(const orientation& this_ori,
                        const circle& other, const orientation& other_ori,
                        std::vector<point>& result) const = 0;
};


struct segment : public shape
{
        point a;
        point b;

        segment(point new_a, point new_b) : a(new_a), b(new_b) {}

        int collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const;

        int collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const;

        int collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const;
};

struct polygon : public shape
{
        std::vector<segment> segs;

        int collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const;

        int collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const;

        int collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const;
};

struct square : public shape
{
        double side;

        int collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const;

        int collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const;

        int collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const;
};

struct circle : public shape
{
        double radius;

        int collide_with(const orientation& this_ori,
                         const shape& other, const orientation& other_ori,
                         std::vector<point>& result) const;

        int collide_with_segment(const orientation& this_ori,
                                 const segment& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_polygon(const orientation& this_ori,
                                 const polygon& other, const orientation& other_ori,
                                 std::vector<point>& result) const;

        int collide_with_square(const orientation& this_ori,
                                const square& other, const orientation& other_ori,
                                std::vector<point>& result) const;

        int collide_with_circle(const orientation& this_ori,
                                const circle& other, const orientation& other_ori,
                                std::vector<point>& result) const;
};

int collide(const shape& a, const orientation& a_ori,
            const shape& b, const orientation& b_ori,
            std::vector<point>& result);

}

#endif
