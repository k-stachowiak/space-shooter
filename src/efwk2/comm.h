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

#ifndef COMM_H
#define COMM_H

#include "../misc/delq.h"

namespace efwk
{

struct bullet_req
{
        double x, y;
        bool is_enemy;
        double damage;
};

struct missile_req
{
        double x, y;
        bool is_enemy;
        double damage;
};

struct spark_req
{
        double x, y;
        double vx, vy;
        std::array<double, 3> rgb;
        double ttl;
};

struct explosion_req
{
        double x;
        double y;
};

struct smoke_req
{
        double x;
        double y;
};

struct death_event
{
        long dying_id;
        long score_id;
};

struct comm_bus
{
        // Low level requests.
        del_queue<bullet_req> bullet_reqs;
        del_queue<missile_req> missile_reqs;
        del_queue<spark_req> spark_reqs;
        del_queue<explosion_req> expl_reqs;
        del_queue<smoke_req> smoke_reqs;
        del_queue<long> del_reqs;

        // Events.
        std::vector<death_event> death_events;

        // Helpers.
        void clear_events()
        {
                death_events.clear();
        }
};

}

#endif
