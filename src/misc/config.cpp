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

#include "exceptions.h"
#include "config.h"

namespace cfg {

std::map<std::string, int> int_vars;
std::map<std::string, double> real_vars;

template<class T>
T get(std::map<std::string, T> const& m, std::string const& key) {
        auto it = m.find(key);
		if (it == end(m)) {
			throw resource_not_found_error("No entry for key " + key + " of a given type found.");
		}
        return it->second;
}

int integer(std::string const& key) {
        return get(int_vars, key);
}

double real(std::string const& key) {
        return get(real_vars, key);
}

loader::loader(const script::scriptman& sman) {
	sman.for_each_integer("config", [](const std::string& name, long value) mutable {
		int_vars[name] = value;
	});
	sman.for_each_real("config", [](const std::string& name, double value) mutable {
		real_vars[name] = value;
	});
}

}
