#ifndef CONFIG_H
#define CONFIG_H

class config {
public:
	double get_fps() const {
		return 50.0;
	}

	double get_screen_w() const {
		return 800.0;
	}

	double get_screen_h() const {
		return 800.0;
	}

	double get_player_vx() const {
		return 400.0f;
	}

	double get_player_vy() const {
		return 300.0f;
	}

	double get_player_max_health() const {
		return 100.0;
	}

	double get_player_max_armor() const {
		return 100.0;
	}

	double get_shoot_period() const {
		return 0.1;
	}

	double get_player_bullet_v() const {
		return 1000.0;
	}

	double get_eye_bullet_v() const {
		return 500.0;
	}

	double get_near_star_v() const {
		return 100.0;
	}

	double get_eye_v() const {
		return 200.0f;
	}
};

#endif
