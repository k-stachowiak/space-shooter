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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::string;

class initialization_error : public runtime_error {
public:
	initialization_error(string message)
	: runtime_error(message) {}
};

class resource_not_found_error : public runtime_error {
public:
	resource_not_found_error(string message)
	: runtime_error(message) {}
};

class parsing_error : public runtime_error {
public:
	parsing_error(string message)
	: runtime_error(message) {}
};

#endif
