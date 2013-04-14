/* Copyright (C) 2012, 2013 Krzysztof Stachowiak */

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

#ifndef PATTERN_H
#define PATTERN_H

#include "../misc/config.h"

enum class enemy_type {
        light_fighter,
        heavy_fighter,
        light_bomber,
        heavy_bomber
};

enum class movement_type {
        zorro,
        diagonal,
        vertical,
        horizontal
};

struct pattern {

        struct element {
                double x_off;
                double y_off;
                enemy_type type;
        };

        vector<element> elements;
        movement_type movement;

        static vector<pattern::element> el_uno(enemy_type type) {
                return {{ 0.0, 0.0, type }};
        }

        static vector<pattern::element> el_pair(enemy_type type) {
                return {{
                                        cfg::real("gameplay_pat_pair_1_xoffset"),
                                        cfg::real("gameplay_pat_pair_1_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_pair_2_xoffset"),
                                        cfg::real("gameplay_pat_pair_2_yoffset"),
                                        type
                                }};
        }

        static vector<pattern::element> el_triangle(enemy_type type) {
                return {{
                                        cfg::real("gameplay_pat_tri_1_xoffset"),
                                        cfg::real("gameplay_pat_tri_1_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_tri_2_xoffset"),
                                        cfg::real("gameplay_pat_tri_2_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_tri_3_xoffset"),
                                        cfg::real("gameplay_pat_tri_3_yoffset"),
                                        type
                                }};
        }

        static vector<pattern::element> el_quad(enemy_type type) {
                return {{
                                        cfg::real("gameplay_pat_quad_1_xoffset"),
                                        cfg::real("gameplay_pat_quad_1_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_quad_2_xoffset"),
                                        cfg::real("gameplay_pat_quad_2_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_quad_3_xoffset"),
                                        cfg::real("gameplay_pat_quad_3_yoffset"),
                                        type
                                },
                                {
                                        cfg::real("gameplay_pat_quad_4_xoffset"),
                                        cfg::real("gameplay_pat_quad_4_yoffset"),
                                        type
                                }};
        }
};

#endif
