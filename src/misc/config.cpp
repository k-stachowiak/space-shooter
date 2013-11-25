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

#include <iostream>
#include <fstream>
#include <map>

#include "../script/parsing.h"
#include "../script/tok.h"
#include "../script/dom.h"
#include "../script/expect.h"
#include "exceptions.h"
#include "config.h"

namespace cfg {

static void register_expression(
                std::string const& key,
                script::dom_node node,
                std::map<std::string, int>& int_vars,
                std::map<std::string, double>& real_vars) {

        // This is a dumb plug that will only accept a multiplication.
        // Will do for now, maybe a full blown scripting language will
        // replace this some day.

        if(!list_size(node, 3))
                throw parsing_error("Only 3 element expression supported.");

        if(!sub_atom(node, 0, "*"))
                throw parsing_error("Only multiplication expression supported.");

        // Analyze the left operand.
        if(!sub_is_atom(node, 1))
                throw parsing_error("First operand of * must be an atom.");

        std::string other_key = node.list[1].atom;

        int ileft = 0;
        double dleft = 0;
        bool left_int = false;

        if(int_vars.find(other_key) != end(int_vars)) {
                ileft = int_vars[other_key];
                left_int = true;
        } else {
                dleft = real_vars[other_key];
        }

        // Analyze the right operand.
        if(!sub_is_atom(node, 2))
                throw parsing_error("Second operand of * must be an atom.");

        std::string literal = node.list[2].atom;

        int iright = 0;
        double dright = 0;
        bool right_int;

        if(!script::parse_literal(literal, right_int, iright, dright))
                throw parsing_error("Cannot parse value \"" + literal + "\".");

        // Perform operation.
        if(left_int && right_int) {
                int_vars[key] = ileft * iright;

        } else {
                double left;
                if(left_int) left = ileft;
                else left = dleft;

                double right;
                if(right_int) right = iright;
                else right = dright;

                real_vars[key] = left * right;
        }
}


static void register_value(
                std::string const& key,
                std::string const& value_str,
                std::map<std::string, int>& int_vars,
                std::map<std::string, double>& real_vars) {

        bool is_int;
        int int_val;
        double real_val;

        if(!script::parse_literal(value_str, is_int, int_val, real_val))
                throw parsing_error("Cannot parse value \"" + value_str + "\".");

        if(is_int)
                int_vars[key] = int_val;

        else
                real_vars[key] = real_val;
}

static void parse_kvp(
                script::dom_node const& kvp,
                std::map<std::string, int>& int_vars,
                std::map<std::string, double>& real_vars) {

// TODO: Test all the expecters for the script
//        script::expect_list<
//                script::expect_atom_cap,
//                script::expect_binop<
//                        script::expect_atom_cap,
//                        script::expect_listkk

        if(!is_list(kvp))
                throw parsing_error("Non-list encountered instead of a kvp.");

        if(!list_size(kvp, 2))
                throw parsing_error("Invalid size of a kvp list encountered.");

        if(!sub_is_atom(kvp, 0))
                throw parsing_error("Non-atom node encountered as kvp key.");

        std::string key = kvp.list[0].atom;

        if(sub_is_atom(kvp, 1))
                register_value( key,
                                kvp.list[1].atom,
                                int_vars,
                                real_vars);
        else
                register_expression(
                                key,
                                kvp.list[1],
                                int_vars,
                                real_vars);
}

static void parse_dom(
                script::dom_node const& tree,
                std::map<std::string, int>& int_vars,
                std::map<std::string, double>& real_vars) {

        if(!check(script::expect_list<>(), tree))
                throw parsing_error("Main config node isn't a list.");

        for(script::dom_node const& kvp : tree.list)
                parse_kvp(kvp, int_vars, real_vars);
} 

std::map<std::string, int> int_vars;
std::map<std::string, double> real_vars;

template<class T>
T get(std::map<std::string, T> const& m, std::string const& key) {
        auto it = m.find(key);
        if(it == end(m))
                throw resource_not_found_error(
                                "No entry for key " + key + " of a given type found.");
        return it->second;
}

int integer(std::string const& key) {
        return get(int_vars, key);
}

double real(std::string const& key) {
        return get(real_vars, key);
}

loader::loader(script::dom_node const& n) {
        parse_dom(n, int_vars, real_vars);
}

}
