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

#ifndef DELQ_H
#define DELQ_H

#include <vector>
#include <utility>

template<class T>
class del_queue
{
        std::vector<std::pair<double, T>> _impl;

public:
        void push(T const& val, double delay)
        {
                _impl.emplace_back(delay, val);
        }

        void push(T const& val)
        {
                push(val, 0.0);
        }

        void consume(del_queue<T>& other)
        {
                _impl.insert(end(_impl),
                                begin(other._impl),
                                end(other._impl));
                other._impl.clear();
        }

        template<class Callback>
        void visit(double dt, Callback cb)
        {
                for(unsigned i = 0; i < _impl.size();) {
                        auto& pr = _impl[i];
                        if(pr.first > 0.0) {
                                pr.first -= dt;
                                ++i;
                        } else {
                                cb(pr.second);
                                _impl[i] = std::move(_impl.back());
                                _impl.pop_back();
                                // Note: no increment in this case.
                        }
                }
        }
};

#endif
