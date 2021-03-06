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

#ifndef MAYBE_H
#define MAYBE_H

#include <utility>

template<class T>
class maybe {
        bool _valid;
        T _value;
public:
        maybe() : _valid(false), _value(T()) {}
        maybe(T value) : _valid(true), _value(std::move(value)) {}
        void reset() { _valid = false; }
        T const& get() const { return _value; }
        operator bool() { return _valid; }
};


#endif /* MAYBE_H_ */
