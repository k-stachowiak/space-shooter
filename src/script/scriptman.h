/* Copyright (C) 2012 Krzysztof Stachowiak */

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

#ifndef SCRIPTMAN_H
#define SCRIPTMAN_H

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <functional>

#include "LIB/API/MoonEngine.h"
#include "LIB/API/Value.h"

namespace script {

        class scriptman {
			moon::CEngine _engine;

        public:
            scriptman(std::set<std::string> files) {
				for (const auto& filename : files) {
					_engine.LoadUnitFile(filename);
                }
            }

			template<class Func>
			void for_each_integer(const std::string& unit_name, Func f) const {
				auto value_names = _engine.GetAllValues(unit_name);
				for (const auto& value_name : value_names) {
					moon::CValue value = _engine.GetValue(unit_name, value_name);
					if (IsInteger(value)) {
						f(value_name, value.GetInteger());
					}
				}
			}

			template<class Func>
			void for_each_real(const std::string& unit_name, Func f) const {
				auto value_names = _engine.GetAllValues(unit_name);
				for (const auto& value_name : value_names) {
					moon::CValue value = _engine.GetValue(unit_name, value_name);
					if (IsReal(value)) {
						f(value_name, value.GetReal());
					}
				}
			}

			std::vector<std::vector<moon::CValue>> read_wave_tuple_array(
					const std::string& unit_name,
					const std::string& symbol) const {

				auto waves = _engine.GetValue(unit_name, symbol);
				if (!IsArray(waves)) {
					return{};
				}

				std::vector<std::vector<moon::CValue>> result;
				for (const auto& wave : waves.GetCompound()) {
					if (!IsArray(wave)) {
						return{};
					}

					std::vector<moon::CValue> patterns;
					for (const auto& pattern : wave.GetCompound()) {

						if (!IsTuple(pattern, 4) ||
							!IsReal(pattern.GetCompound()[0]) ||
							!IsString(pattern.GetCompound()[1]) ||
							!IsString(pattern.GetCompound()[2]) ||
							!IsString(pattern.GetCompound()[3])) {
							return{};
						}

						patterns.push_back(pattern);
					}

					result.push_back(patterns);
				}

				return result;
			}
        };

} // namespace script

#endif
