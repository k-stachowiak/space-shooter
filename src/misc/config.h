/* Copyright (C) 2012,2013 Krzysztof Stachowiak */

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

#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#include <string>
using std::string;

namespace cfg {

        int integer(string const& key);
        double real(string const& key);

        void load_from_file(string const& name);

        namespace math {
                const double pi = 3.1415;
                const double two_pi = 6.2830;
                const double half_pi = 1.577;
        }

        namespace type_bounds {
                const size_t max_num_frames = 128;
                const size_t max_frame_defs = 128;
        }

}

#endif