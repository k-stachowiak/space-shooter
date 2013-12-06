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
#include <string>

struct initialization_error : public std::runtime_error {
        initialization_error(std::string message)
        : std::runtime_error(message) {}
};

struct resource_not_found_error : public std::runtime_error {
        resource_not_found_error(std::string message)
        : std::runtime_error(message) {}
};

struct parsing_error : public std::runtime_error {
        parsing_error(std::string message)
        : std::runtime_error(message) {}
};

struct fatal_error : public std::runtime_error {
        fatal_error(std::string message)
        : std::runtime_error(message) {}
};

#endif
