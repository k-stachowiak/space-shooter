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

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "sys_hud.h"

/*
 * This file contains a lot of magic numbers, but the HUD is defined
 * "pixel perfect", so it's not really configurable. Let's hope this
 * will never have to change.
 */

ALLEGRO_COLOR health_color(double health_ratio) {
	double r, g;
	if(health_ratio >= 0.5) {
		g = 1.0;
		r = 1.0 - (health_ratio - 0.5) * 2.0;
	} else {
		r = 1.0;
		g = health_ratio * 2.0;
	}
	return al_map_rgb_f(r, g, 0.0);
}

void draw_bar(
		double screen_w, double screen_h,
		double from_bottom,
		double ratio,
		double thickness,
		double margin,
		ALLEGRO_COLOR color) {
	double x1 = screen_w * 0.15 - margin;
	double x2 = screen_w * 0.9 + margin;
	double y1 = screen_h - from_bottom - thickness - margin;
	double y2 = screen_h - from_bottom + margin;
	double hx2 = x1 + ratio * (x2 - x1);
	al_draw_filled_rectangle(x1, y1, hx2, y2, color);
}

namespace sys {

void hud_system::draw_background() {
	al_draw_bitmap(_hud_bg, 0, 0, 0);
}

void hud_system::draw_score(nd::hud_node const& n) {
	int player_score = int(*(n.score));
	al_draw_textf(
		_score_font,
		al_map_rgba_f(1.0f, 1.0f, 1.0f, 1.0f),
		10.0f, 10.0f, 0,
		"Score: %d", player_score);
}

void hud_system::draw_wellness(nd::hud_node const& n) {
	double max_health = n.wellness->get_max_health();
	double health = n.wellness->get_health();
	double h_ratio = health / max_health;
	if(h_ratio < 0.0) h_ratio = 0.0;
	draw_bar(_screen_w, _screen_h, 20.0, 1.0, 5.0, 2.0, al_map_rgb(0, 0, 0));
	draw_bar(_screen_w, _screen_h, 20.0, h_ratio, 5.0, 0.0, health_color(h_ratio));

	double max_shield = n.wellness->get_max_shield();
	double shield = n.wellness->get_shield();
	double s_ratio = shield / max_shield;
	if(s_ratio < 0.0) s_ratio = 0.0;
	draw_bar(_screen_w, _screen_h, 50.0, 1.0, 5.0, 2.0, al_map_rgb(0, 0, 0));
	draw_bar(_screen_w, _screen_h, 50.0, s_ratio, 5.0, 0.0, al_map_rgb(113, 197, 255));

	al_draw_bitmap(_hud_health, 75.0, _screen_h - 20.0 - 20.0, 0);
	al_draw_bitmap(_hud_battery, 75.0, _screen_h - 50.0 - 20.0, 0);
}

void hud_system::draw_upgrades(nd::hud_node const& n) {
	size_t gun_lvl = n.upgrades->gun_lvl();
	size_t gun_ticks = n.upgrades->gun_ticks();
	size_t gun_max_ticks = n.upgrades->gun_ticks_per_level();

	size_t rl_lvl = n.upgrades->rl_lvl();
	size_t rl_ticks = n.upgrades->rl_ticks();
	size_t rl_max_ticks = n.upgrades->rl_ticks_per_level();

	auto don = _hud_diode_on;
	auto doff = _hud_diode_off;

	double y_base = _screen_h - 22.0 - al_get_bitmap_height(doff) / 2;
	double x_left = 23.0 - al_get_bitmap_width(doff) / 2;
	double x_right = _screen_w - 23.0 - al_get_bitmap_width(doff) / 2;

	al_draw_bitmap((gun_lvl > 1) ? don : doff, x_left, y_base - 30, 0);
	al_draw_bitmap((gun_lvl > 2) ? don : doff, x_left, y_base - 50, 0);
	al_draw_bitmap((gun_lvl > 3) ? don : doff, x_left, y_base - 70, 0);
	al_draw_bitmap((gun_lvl > 4) ? don : doff, x_left, y_base - 90, 0);

	al_draw_bitmap((rl_lvl > 1) ? don : doff, x_right, y_base - 30, 0);
	al_draw_bitmap((rl_lvl > 2) ? don : doff, x_right, y_base - 50, 0);
	al_draw_bitmap((rl_lvl > 3) ? don : doff, x_right, y_base - 70, 0);
	al_draw_bitmap((rl_lvl > 4) ? don : doff, x_right, y_base - 90, 0);

	// NOTE: This system will light n diodes for n-1 upgrade level.
	// This way the initial upgrade levels = 1 are indicated by no diodes.

	al_draw_bitmap(_b_upgrade, x_left + 2.0, y_base, 0);
	al_draw_bitmap(_m_upgrade, x_right + 2.0, y_base, 0);

	double gun_ticks_ratio = (double)gun_ticks / (double)gun_max_ticks;
	double rl_ticks_ratio = (double)rl_ticks / (double)rl_max_ticks;

	double y_from = y_base;
	double y_to = y_base - 90.0;

	double y_gun_inter = y_base - 90.0 * gun_ticks_ratio;
	double y_rl_inter = y_base - 90.0 * rl_ticks_ratio;

	al_draw_line(x_left, y_from, x_left, y_to, al_map_rgb_f(0,0,0), 8);
	al_draw_line(x_right, y_from, x_right, y_to, al_map_rgb_f(0,0,0), 8);
	al_draw_line(x_left, y_from, x_left, y_gun_inter, al_map_rgb_f(1,1,0), 4);
	al_draw_line(x_right, y_from, x_right, y_rl_inter, al_map_rgb_f(1,1,0), 4);
}


void hud_system::update() {
	// This loop is a sloppy replacement for a "maybe type"
	// if the vector is empty, nothing will be drawn, otherwise
	// we will see the HUD for all the nodes defined here.
	// Only one node should be ever added to this lousy excuse
	// for a system.
	for(auto const& n : _nodes) {
		draw_background();
		draw_score(n);
		draw_wellness(n);
		draw_upgrades(n);
	}
}

}
