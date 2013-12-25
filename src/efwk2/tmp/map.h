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

#ifndef MAP_H
#define MAP_H

#include <algorithm>

#include "traits.h"

namespace efwk
{

// Map function to argument bag.
// -----------------------------

template <class Func>
void map(Func)
{
}

template <class Func, class Head, class... Tail>
typename std::enable_if<IsCollection<Head>::value, void>::type
map(Func func, Head& head, Tail&... tail)
{
        std::for_each(begin(head), end(head), func);
        map(func, tail...);
}

template <class Func, class Head, class... Tail>
typename std::enable_if<!IsCollection<Head>::value, void>::type
map(Func func, Head& head, Tail&... tail)
{
        func(head);
        map(func, tail...);
}

// Map funcrion to all pairs of elements in argument bag.
// ------------------------------------------------------

// TODO: consider refactoring to reduce the number of cases to consired.

template <class Func, class First>
typename std::enable_if<IsCollection<First>::value, void>::type
map2_impl(Func func, First& first)
{
        for (auto i = begin(first); i != end(first); ++i)
                for (auto j = (i + 1); j != end(first); ++j)
                        func(*i, *j);
}

template <class Func, class First>
typename std::enable_if<!IsCollection<First>::value, void>::type
map2_impl(Func, First&)
{
}

template <class Func, class First, class Second>
typename std::enable_if<IsCollection<First>::value &&
                        IsCollection<Second>::value, void>::type
map2_impl(Func func, First& first, Second& second)
{
        for (auto& i : first)
                for (auto& j : second)
                        func(i, j);
}

template <class Func, class First, class Second>
typename std::enable_if<!IsCollection<First>::value &&
                        IsCollection<Second>::value, void>::type
map2_impl(Func func, First& first, Second& second)
{
        for (auto& j : second)
                func(first, j);
}

template <class Func, class First, class Second>
typename std::enable_if<IsCollection<First>::value &&
                        !IsCollection<Second>::value, void>::type
map2_impl(Func func, First& first, Second& second)
{
        map2_impl(second, first);
}

template <class Func, class First, class Second>
typename std::enable_if<!IsCollection<First>::value &&
                        !IsCollection<Second>::value, void>::type
map2_impl(Func func, First& first, Second& second)
{
        func(first, second);
}

template <class Func, class First>
void map2_first_rest(Func, First&)
{
}

template <class Func, class First, class Second, class... Rest>
void map2_first_rest(Func func, First& first, Second& second, Rest&... rest)
{
        map2_impl(func, first, second);
        map2_first_rest(func, first, rest...);
}

template <class Func, class First>
void map2(Func func, First& first)
{
        map2_impl(func, first);
}

template <class Func, class First, class Second, class... Rest>
void map2(Func func, First& first, Second& second, Rest&... rest)
{
        map2_impl(func, first);
        map2_first_rest(func, first, second, rest...);
        map2(func, second, rest...);
}

}

#endif
