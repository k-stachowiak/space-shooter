#include "systems.h"

namespace sys {

	// Drawing system.
	// ---------------

	void drawing_system::update(double dt) {
		al_clear_to_color(al_map_rgb_f(0.0f, 0.0f, 0.0f));

		ALLEGRO_BITMAP* bmp;
		double x, y;
		double theta;
		for(auto const& n : _nodes) {
			n.appearance->update(dt);
			bmp = n.appearance->bitmap();
			x = n.orientation->get_x();
			y = n.orientation->get_y();
			theta = n.orientation->rotation();
			int w = al_get_bitmap_width(bmp);
			int h = al_get_bitmap_height(bmp);
			al_draw_rotated_bitmap(bmp, 
					w >> 1, h >> 1,
					x, y, theta, 0);
		}
	}

	// Movement system.
	// ----------------

	void movement_system::update(
			double dt,
			vector<comm::message>& msgs) {
		double vx, vy;
		double x, y;
		double mini, maxi;
		for(auto const& n : _nodes) {

			// Determine the velocities.
			if(n.identity == _player_controlled) {
				vx = _player_throttle_x * 400.0;
				vy = _player_throttle_y * 300.0;
			} else {
				n.dynamics->update(dt);
				vx = n.dynamics->get_vx();
				vy = n.dynamics->get_vy();
			}

			// Read current position.
			x = n.orientation->get_x();
			y = n.orientation->get_y();

			// Perform bounded or unbounded movement.
			//
			// Note that in both cases the x and y variables
			// are updated even though it is not necessary in
			// case of the unbounded movement. They are changed
			// bacause they're used later in the life bounds check.
			double dx = 0.0;
			double dy = 0.0;
			if(n.movement_bounds) {
				mini = n.movement_bounds->get_x_min();
				maxi = n.movement_bounds->get_x_max();
				if(between((x += vx * dt), mini, maxi))
					dx = vx * dt;

				mini = n.movement_bounds->get_y_min();
				maxi = n.movement_bounds->get_y_max();
				if(between((y += vy * dt), mini, maxi))
					dy = vy * dt;

			} else {
				dx = vx * dt;
				dy = vy * dt;
			}

			// Perform the actual move.
			n.orientation->set_x(x += dx);
			n.orientation->set_y(y += dy);

			if(n.shape)
				n.shape->shift(dx, dy);

			// Check the life boundaries.
			if(n.life_bounds) {
				mini = n.life_bounds->get_x_min();
				maxi = n.life_bounds->get_x_max();
				if(!between(x, mini, maxi)) {
					msgs.push_back(comm::create_remove_entity(
								n.identity));
					return;
				}
				mini = n.life_bounds->get_x_min();
				maxi = n.life_bounds->get_y_max();
				if(!between(y, mini, maxi)) {
					msgs.push_back(comm::create_remove_entity(
								n.identity));
					return;
				}
			}
		}
	}

	// Arms system.
	// ------------

	void arms_system::handle_player(double dt,
			vector<comm::message>& msgs,
			double x,
			double y) {

		if(!_player_trigger) {
			_player_counter = 0.0;
			return;
		}

		_player_counter -= dt;
		if(_player_counter > 0.0)
			return;

		_player_counter += _player_interval;
		msgs.push_back(comm::create_spawn_bullet(
					x, y, -1.57, 0.0, -800.0,
					cmp::coll_class::PLAYER_BULLET));
	}

	void arms_system::update(double dt, vector<comm::message>& msgs) {
		double x, y;
		for(auto const& n : _nodes) {
			x = n.orientation->get_x();
			y = n.orientation->get_y();
			if(n.identity == _player_shooting) {
				handle_player(dt, msgs, x, y);
				return;
			}
		}
	}

	// Collisions system.
	// ------------------

	void collision_system::check_collision(nd::collision_node const& a,
			     nd::collision_node const& b) {

		cmp::shape const& shp_a = *(a.shape);
		cmp::shape const& shp_b = *(b.shape);

		if(shp_a.collides_with(shp_b)) {
			cmp::coll_report report {
				a.cc, a.shape, b.cc, b.shape };
			a.coll_queue->push_report(report);
			b.coll_queue->push_report(report);
		}
	}

	void collision_system::update() {
		// Clear the collision queues first.
		for(auto a = begin(_nodes); a != end(_nodes); ++a)
			a->coll_queue->clear();

		// Perform the collision checks.
		for(auto a = begin(_nodes); a != end(_nodes); ++a)
			for(auto b = a + 1; b != end(_nodes); ++b)
				check_collision(*a, *b);

	}

	// Pain system.
	// ------------
	
	void pain_system::update(vector<comm::message>& msgs) {
		for(auto const& n : _nodes) {

			auto fun = [&n, &msgs](cmp::coll_report const& r) {

				auto const& other_cc = (r.cc_a == n.cc)
					? r.cc_b
					: r.cc_a;

				double pain = n.painmap->get_pain(other_cc);
				n.wellness->deal_dmg(pain);
				if(!n.wellness->is_alive())
					msgs.push_back(comm::create_remove_entity(
							n.identity));

				// TODO: Move the death decision into the death system.
			};

			n.coll_queue->for_each_report(fun);
		}
	}
}
