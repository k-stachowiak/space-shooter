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

#include <string>

#include "../script/dom.h"

// TODO: Don't evaluate missing symbol to 0!

namespace cfg {

        // Main API.
        // ---------

        int integer(std::string const& key);
        double real(std::string const& key);

        namespace math {
                const double pi = 3.1415;
                const double two_pi = 6.2830;
                const double half_pi = 1.577;
        }

        namespace type_bounds {
                const unsigned max_num_frames = 128;
                const unsigned max_frame_defs = 128;
        }

        // Helper type (for integration with RAII).
        // ----------------------------------------

        // Note: WTF did I just come up with ?!

        struct loader {
                loader(script::dom_node const& n);
        };
}

#endif
