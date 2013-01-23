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

#include "resman.h"

resman::resman(ALLEGRO_DISPLAY* dpy)
: _dpy(dpy) {

	// Initialize all the resources to be needed later here.
	// -----------------------------------------------------
	
	// Load simple images.
	add_bitmap(res_id::PLAYER_SHIP, "data/ship_from_internetz.png");
	add_bitmap(res_id::PLAYER_BULLET, "data/player_bullet.png");
	add_bitmap(res_id::ENEMY_EYE, "data/enemy_eye.png");
	add_bitmap(res_id::ENEMY_BOMBER, "data/enemy_bomber2.png");
	add_bitmap(res_id::EYE_BULLET, "data/eye_bullet.png");
	add_bitmap(res_id::MISSILE, "data/missile.png");
	add_bitmap(res_id::MISSILES, "data/missiles.png");
	add_bitmap(res_id::DEBRIS1, "data/debris1.png");
	add_bitmap(res_id::DEBRIS2, "data/debris2.png");
	add_bitmap(res_id::DEBRIS3, "data/debris3.png");
	add_bitmap(res_id::DEBRIS4, "data/debris4.png");
	add_bitmap(res_id::DEBRIS5, "data/debris5.png");
	add_bitmap(res_id::HEALTH, "data/health.png");

	// Load/generate animations
	expand_fade(res_id::SMOKE, "data/smoke_single.png", 15, 3.0);
	scaled_copy(res_id::SMOKE_SMALL, res_id::SMOKE, 0.5);
	scaled_copy(res_id::SMOKE_BIG, res_id::SMOKE, 2.0);
	fade_frames(res_id::EXPLOSION, "data/explosion.png", 16);

	// Generate flashes.
	flash(res_id::PLAYER_SHIP_FLASH, res_id::PLAYER_SHIP);
	flash(res_id::ENEMY_EYE_FLASH, res_id::ENEMY_EYE);
	flash(res_id::ENEMY_BOMBER_FLASH, res_id::ENEMY_BOMBER);

	// Load fonts.
	add_font(res_id::TINY_FONT, "data/prstartk.ttf", 10);
	add_font(res_id::FONT, "data/prstartk.ttf", 24);
}

ALLEGRO_BITMAP* resman::get_bitmap(res_id id) const {

	if(_bitmaps.find(id) == end(_bitmaps)) {
		return nullptr;
	}

	return _bitmaps.at(id).get();
}

ALLEGRO_FONT* resman::get_font(res_id id) const {
	if(_fonts.find(id) == end(_fonts)) {
		return nullptr;
	}

	return _fonts.at(id).get();
}

void resman::add_bitmap(res_id id, string path) {

	if(_bitmaps.find(id) != end(_bitmaps)) {
		throw initialization_error("Loading resource at duplicate id");
	}

	p_bmp bitmap(al_load_bitmap(path.c_str()));

	if(!bitmap) {
		stringstream msg;
		msg << "Failed loading image at " << path;
		throw initialization_error(msg.str());
	}

	_bitmaps[id] = move(bitmap);
}

void resman::add_font(res_id id, string path, int size) {
	if(_fonts.find(id) != end(_fonts)) {
		throw initialization_error("Loading resource at duplicate id");
	}

	unique_ptr<ALLEGRO_FONT, font_deleter> font(al_load_font(
		path.c_str(), -size, 0));

	if(!font) {
		stringstream msg;
		msg << "Failed loading font at " << path;
		throw initialization_error(msg.str());
	}

	_fonts[id] = move(font);
}

void resman::expand_fade(
		res_id id,
		string path,
		uint32_t num_frames,
		double exp_factor) {

	// Load the bitmap from file.
	// --------------------------
	if(_bitmaps.find(id) != end(_bitmaps))
		throw initialization_error("Loading resource at duplicate id");

	p_bmp base_bitmap(al_load_bitmap(path.c_str()));

	if(!base_bitmap) {
		stringstream msg;
		msg << "Failed loading image at " << path;
		throw initialization_error(msg.str());
	}

	// Basic analysis.
	// ---------------
	double orig_w = al_get_bitmap_width(base_bitmap.get()); 
	double orig_h = al_get_bitmap_height(base_bitmap.get()); 
	double frame_w = orig_w * exp_factor;
	double frame_h = orig_h * exp_factor;

	// Generate the target bitmap.
	// ---------------------------
	p_bmp result_bitmap(al_create_bitmap(frame_w * num_frames, frame_h));
	al_set_target_bitmap(result_bitmap.get());
	al_clear_to_color(al_map_rgba_f(0.0f, 0.0f, 0.0f, 0.0f));
	for(uint32_t i = 0; i < num_frames; ++i) {

		// Transformations' progress.
		double grade = (double)i / (double)(num_frames - 1);
		double scale = 1.0 + grade * (exp_factor - 1.0);
		double alpha = 1.0 - grade;

		// Determine the destination tint.
		ALLEGRO_COLOR tint = al_map_rgba_f(alpha, alpha, alpha, alpha);

		// Determine the destination position.
		double dw = orig_w * scale;
		double dh = orig_h * scale;
		double frame_x = (double)i * frame_w;
		double frame_y = 0;
		double dx = frame_x + (frame_w - dw) * 0.5f;
		double dy = frame_y + (frame_h - dh) * 0.5f;

		// Draw the frame.
		al_draw_tinted_scaled_bitmap(
				base_bitmap.get(),
				tint,
				0.0f, 0.0f, orig_w, orig_h,	// Original parameters.
				dx, dy, dw, dh,			// Computed destination.
				0);
	}
	al_set_target_bitmap(al_get_backbuffer(_dpy));

	_bitmaps[id] = move(result_bitmap);
}

void resman::scaled_copy(res_id id, res_id original, double scale) {

	// Preconditions.
	if(_bitmaps.find(id) != end(_bitmaps))
		throw initialization_error("Loading resource at duplicate id");

	if(_bitmaps.find(original) == end(_bitmaps))
		throw initialization_error("Copying resource from nonexistent id");

	// Base dimensions.
	double orig_w = al_get_bitmap_width(_bitmaps[original].get()); 
	double orig_h = al_get_bitmap_height(_bitmaps[original].get()); 

	// Begin drawing to target.
	p_bmp result_bitmap(al_create_bitmap(orig_w * scale, orig_h * scale));
	al_set_target_bitmap(result_bitmap.get());
	al_clear_to_color(al_map_rgba_f(0.0f, 0.0f, 0.0f, 0.0f));

	// Draw.
	al_draw_scaled_bitmap(
			_bitmaps[original].get(),
			0.0f, 0.0f,
			orig_w, orig_h,
			0.0, 0.0,
			orig_w * scale,
			orig_h * scale,
			0);

	// End drawing to target.
	al_set_target_bitmap(al_get_backbuffer(_dpy));
	_bitmaps[id] = move(result_bitmap);
}

void resman::flash(res_id id, res_id original) {
	
	// Base dimensions.
	double orig_w = al_get_bitmap_width(_bitmaps[original].get()); 
	double orig_h = al_get_bitmap_height(_bitmaps[original].get()); 

	// Store blender.
	int op, src, dst;
	al_get_blender(&op, &src, &dst);

	// Begin drawing to target.
	p_bmp result_bitmap(al_create_bitmap(orig_w, orig_h));
	al_set_target_bitmap(result_bitmap.get());
	al_clear_to_color(al_map_rgba_f(1, 1, 1, 0));

	al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA);
	al_draw_bitmap(_bitmaps[original].get(), 0, 0, 0);

	// End drawing to target.
	al_set_target_bitmap(al_get_backbuffer(_dpy));
	_bitmaps[id] = move(result_bitmap);

	// Restore blender.
	al_set_blender(op, src, dst);
}

void resman::fade_frames(res_id id,
			 string path,
			 uint32_t num_frames) {

	// Load the bitmap from file.
	// --------------------------
	if(_bitmaps.find(id) != end(_bitmaps))
		throw initialization_error("Loading resource at duplicate id");

	p_bmp base_bitmap(al_load_bitmap(path.c_str()));

	if(!base_bitmap) {
		stringstream msg;
		msg << "Failed loading image at " << path;
		throw initialization_error(msg.str());
	}

	// Basic analysis.
	// ---------------
	double orig_w = al_get_bitmap_width(base_bitmap.get()); 
	double orig_h = al_get_bitmap_height(base_bitmap.get()); 
	double frame_w = orig_w / num_frames;
	double frame_h = orig_h;

	// Generate the target bitmap.
	// ---------------------------
	p_bmp result_bitmap(al_create_bitmap(orig_w, orig_h));
	al_set_target_bitmap(result_bitmap.get());
	al_clear_to_color(al_map_rgba_f(0.0f, 0.0f, 0.0f, 0.0f));
	for(uint32_t i = 0; i < num_frames; ++i) {

		// Transformations' progress.
		double grade = (double)i / (double)(num_frames - 1);
		double alpha = 1.0 - grade;

		// Determine the destination tint.
		ALLEGRO_COLOR tint = al_map_rgba_f(alpha, alpha, alpha, alpha);

		// Determine the destination position.
		double w = frame_w;
		double h = frame_h;
		double x = (double)i * frame_w;
		double y = 0;

		// Draw the frame.
		al_draw_tinted_scaled_bitmap(
				base_bitmap.get(),
				tint,
				x, y, w, h,	// Original parameters.
				x, y, w, h,	// Computed destination.
				0);
	}
	al_set_target_bitmap(al_get_backbuffer(_dpy));

	_bitmaps[id] = move(result_bitmap);
}

