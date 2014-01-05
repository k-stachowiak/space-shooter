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

#ifndef FLASH_H
#define FLASH_H

#include "ucmp.h"

namespace efwk
{

struct flash
{
        double interval;
        double counter;

        flash(double new_interval) :
                interval(new_interval),
                counter(0)
        {}

        void reset()
        {
                counter = interval;
        }

        void update(double dt)
        {
                if (counter > 0)
                        counter -= dt;
        }

        bool is_flashing() const
        {
                return counter > 0;
        }
};

SFINAE__DECLARE_HAS_MEMBER(HasFlash, flash, flsh);

}

#endif
