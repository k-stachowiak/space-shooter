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

#ifndef COLL_QUEUE_H
#define COLL_QUEUE_H

#include <functional>
#include <algorithm>
#include <memory>
#include <vector>

#include "col_report.h"

namespace cmp {

    // Collision queue.
    class coll_queue {
        std::vector<coll_report> _queue;
    public:
            void clear() { _queue.clear(); }
            void push_report(coll_report cr) { _queue.push_back(cr); }
            void for_each_report(std::function<void(coll_report const&)> f) const {
                std::for_each(begin(_queue), end(_queue), f);
            }
    };

    std::shared_ptr<coll_queue> create_coll_queue();

}

#endif
