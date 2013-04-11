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

#ifndef DOM_H
#define DOM_H

#include <iostream>
#include <string>
#include <vector>

#include "tok.h"

namespace script {

        enum class dom_node_t { atom, list };

        struct dom_node {
                dom_node_t type;
                std::string atom;
                std::vector<dom_node> list;
        };

        std::ostream& operator<<(std::ostream& out, dom_node const& n);

        dom_node build_dom_tree(tokenizer& tok);

        static bool is_atom(dom_node const& n) { return n.type == dom_node_t::atom; }
        static bool is_list(dom_node const& n) { return n.type == dom_node_t::list; }
        static bool list_size(dom_node const& n, unsigned size) {
                return n.list.size() == size;
        }
        static bool sub_is_atom(dom_node const& n, unsigned index) {
                return n.list[index].type == dom_node_t::atom;
        }
        static bool sub_atom(dom_node const& n, unsigned index, string const& str) {
                return n.list[index].atom == str;
        }
        static bool sub_is_list(dom_node const& n, unsigned index) {
                return n.list[index].type == dom_node_t::list;
        }
}

#endif
