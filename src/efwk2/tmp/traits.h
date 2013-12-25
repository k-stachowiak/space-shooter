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

#ifndef TRAITS_H
#define TRAITS_H

namespace efwk
{

// Simple traits.
// --------------

template <class T>
struct IsCollection : std::false_type {};

template <class T>
struct IsCollection<std::vector<T>> : std::true_type {};

template <class T>
struct IsCollection<std::vector<T>&> : std::true_type {};

// Trait combinators.
// ------------------

template<class Head, class... Tail>
struct TmpAll
{
        static bool const value =
                Head::value &&
                TmpAll<Tail...>::value;
};

template<class Head>
struct TmpAll<Head>
{
        static bool const value = Head::value;
};

template<class Head, class... Tail>
struct TmpAny
{
        static bool const value =
                Head::value ||
                TmpAny<Tail...>::value;
};

template<class Head>
struct TmpAny<Head>
{
        static bool const value = Head::value;
};


}

#endif
