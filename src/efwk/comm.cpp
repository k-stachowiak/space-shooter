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

#include "comm.h"

namespace comm {

message create_remove_entity(uint64_t id) {
        message msg;
        msg.type = msg_t::remove_entity;
        msg.remove_entity = { id };
        return msg;
}

message create_spawn_bullet(
                double x, double y,
                double dir_x, double dir_y,
                double lin_vel,
                unsigned upgrade_lvl,
                bool enemy,
                uint64_t origin_id) {
        message msg;
        msg.type = msg_t::spawn_bullet;
        msg.spawn_bullet = { x, y, dir_x, dir_y, lin_vel, upgrade_lvl, enemy, origin_id };
        return msg;
}

message create_spawn_missile(
                double x, double y,
                double dir_x, double dir_y,
                double lin_vel,
                unsigned upgrade_lvl,
                bool enemy,
                uint64_t origin_id) {
        message msg;
        msg.type = msg_t::spawn_missile;
        msg.spawn_missile = { x, y, dir_x, dir_y, lin_vel, upgrade_lvl, enemy, origin_id };
        return msg;
}

message create_spawn_explosion(double x, double y) {
        message msg;
        msg.type = msg_t::spawn_explosion;
        msg.spawn_explosion = { x, y };
        return msg;
}

message create_spawn_smoke(double x, double y, smoke_size size) {
        message msg;
        msg.type = msg_t::spawn_smoke;
        msg.spawn_smoke = { x, y, size };
        return msg;
}

message create_spawn_debris(
                double x, double y,
                double vx, double vy,
                double vmin, double vmax,
                double theta_min, double theta_max,
                res::res_id image,
                bool explode,
                uint64_t origin_id) {
        message msg;
        msg.type = msg_t::spawn_debris;
        msg.spawn_debris = { x, y,
                vx, vy,
                vmin, vmax,
                theta_min, theta_max,
                image,
                explode,
                origin_id };
        return msg;
}

message create_spawn_health_pickup(
                double x, double y,
                double vx, double vy) {
        message msg;
        msg.type = msg_t::spawn_health_pickup;
        msg.spawn_health_pickup = { x, y, vx, vy };
        return msg;
}

message create_spawn_battery_pickup(
                double x, double y,
                double vx, double vy) {
        message msg;
        msg.type = msg_t::spawn_battery_pickup;
        msg.spawn_battery_pickup = { x, y, vx, vy };
        return msg;
}

message create_spawn_bullet_upgrade_pickup(
                double x, double y,
                double vx, double vy) {
        message msg;
        msg.type = msg_t::spawn_bullet_upgrade_pickup;
        msg.spawn_bullet_upgrade_pickup = { x, y, vx, vy };
        return msg;
}

message create_spawn_missile_upgrade_pickup(
                double x, double y,
                double vx, double vy) {
        message msg;
        msg.type = msg_t::spawn_missile_upgrade_pickup;
        msg.spawn_missile_upgrade_pickup = { x, y, vx, vy };
        return msg;
}

}
