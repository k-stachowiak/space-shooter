/* Copyright (C) 2013 Krzysztof Stachowiak */

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

#ifndef SFINAE_H
#define SFINAE_H

#include <type_traits>
#include <algorithm>

namespace efwk
{

// Sweet mother of Jesus, WTF is this!?
// ------------------------------------

#define SFINAE__DECLARE_HAS_MEMBER(IDENTIFIER, TYPE, NAME) \
template<typename T> struct IDENTIFIER \
{ \
        struct Fallback { TYPE NAME; }; \
        struct Derived : T, Fallback {}; \
        template<typename C, C> struct ChT; \
        template<typename C> static char (&f(ChT<TYPE Fallback::*, &C::NAME>*))[1]; \
        template<typename C> static char (&f(...))[2]; \
        static bool const value = sizeof(f<Derived>(0)) == 2; \
}

}

#endif
