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

#include "../tmp/sfinae.h"

namespace efwk
{

struct coll_report
{
        long id;
        const char* type_id;
        std::vector<point> points;
};

class coll_queue
{
        std::vector<coll_report> m_reports;

public:
        void push(coll_report report)
        {
                m_reports.push_back(std::move(report));
        }

        void clear()
        {
                m_reports.clear();
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasCollisionQueue, coll_queue, collq);

}

#endif
