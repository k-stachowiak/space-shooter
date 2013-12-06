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

#ifndef COL_REPORT_H
#define COL_REPORT_H

#include <memory>

namespace cmp {

    class shape;
    class collision_profile;
    class pickup_profile;

    // Collision report type.
    //
    // Note: The origin id is the identifier of the node which
    // deals damage through this node. E.g. If this node is a
    // bullet, the origin_id will be that of the ship, who has
    // launched the bullet. This way the entity responsible
    // for the damage may be determined.
    struct coll_report {
            uint64_t id;
            uint64_t origin_id;
            std::shared_ptr<collision_profile> cp;
            std::shared_ptr<pickup_profile> pp;
            std::shared_ptr<shape> shp;
    };

}

#endif
