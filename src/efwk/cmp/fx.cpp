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

#include <random>
#include <tuple>
#include <iostream>

#include "../../misc/rand.h"

#include "cooldown.h"
#include "timer.h"
#include "shape.h"
#include "fx.h"

namespace cmp {

    class complex_fx : public fx {
            std::vector<std::shared_ptr<fx>> _fxs;
    public:

        complex_fx(std::vector<std::shared_ptr<fx>> const& fxs) : _fxs(fxs) {}

        void update(double dt,
                    double health_ratio,
                    double shield_ratio,
                    double x, double y,
                    cmp::shape const& shape,
                    comm::msg_queue& msgs) {
            for(auto& f : _fxs) {
                f->update(dt, health_ratio, shield_ratio, x, y, shape, msgs);
            }
        }
    };

    class smoke_when_hurt : public fx {
            std::shared_ptr<timer> _timer;
        double _pain_threshold;
    public:
        smoke_when_hurt(std::shared_ptr<timer> t, double pain_threshold)
        : _timer(t)
        , _pain_threshold(pain_threshold)
        {}

        void update(double dt,
                    double health_ratio,
                    double shield_ratio,
                    double x, double y,
                    cmp::shape const& shape,
                    comm::msg_queue& msgs) {

            if(health_ratio > _pain_threshold)
                return;

            _timer->update(dt);
            for(uint32_t i = 0; i < _timer->get_ticks(); ++i) {
                double rnd_x, rnd_y;
                std::tie(rnd_x, rnd_y) = shape.get_random_point();
                msgs.push(comm::create_spawn_smoke(rnd_x + x,
                                                   rnd_y + y,
                                                   comm::smoke_size::medium));
            }
            _timer->clear();
        }
    };

    class period_smoke : public fx {
        double _dt_min;
        double _dt_max;
        double _counter;

        void init_counter(double remainder = 0.0) {
                std::uniform_real_distribution<double> distr(_dt_min, _dt_max);
            _counter = distr(rnd::engine) - remainder;
        }

    public:
        period_smoke(double dt_min, double dt_max)
        : _dt_min(dt_min)
        , _dt_max(dt_max) {
            init_counter();
        }

        void update(double dt,
                    double health_ratio,
                    double shield_ratio,
                    double x, double y,
                    cmp::shape const& shape,
                    comm::msg_queue& msgs) {
            _counter -= dt;
            if(_counter <= 0.0) {
                init_counter(-_counter);
                msgs.push(comm::create_spawn_smoke(x, y, comm::smoke_size::tiny));
            }
        }
    };

    class flash_when_hurt : public fx {

        std::shared_ptr<bool> _flag;
        cooldown _cd;
        double _last_health_ratio;
        double _last_shield_ratio;

    public:

        flash_when_hurt(std::shared_ptr<bool> flag, double cooldown_time)
        : _flag(flag)
        , _cd(cooldown_time)
        , _last_health_ratio(-1)
        , _last_shield_ratio(-1)
        {}

        void update(double dt,
                    double health_ratio,
                    double shield_ratio,
                    double x, double y,
                    cmp::shape const& shape,
                    comm::msg_queue& msgs) {

            if (shield_ratio < _last_shield_ratio ||
                health_ratio < _last_health_ratio) {
                    _cd.reset();
            }

            *(_flag) = _cd.is_cooldown();
            _cd.update(dt);

            _last_health_ratio = health_ratio;
            _last_shield_ratio = shield_ratio;
        }

    };

    std::shared_ptr<fx> create_complex_fx(std::vector<std::shared_ptr<fx>> const& fxs) {
        return std::shared_ptr<fx>(new complex_fx(fxs));
    }

    std::shared_ptr<fx> create_smoke_when_hurt(std::shared_ptr<timer> t, double pain_threshold) {
        return std::shared_ptr<fx>(new smoke_when_hurt(t, pain_threshold));
    }

    std::shared_ptr<fx> create_period_smoke(double dt_min, double dt_max) {
        return std::shared_ptr<fx>(new period_smoke(dt_min, dt_max));
    }

    std::shared_ptr<fx> create_flash_when_hurt(std::shared_ptr<bool> flag, double cooldown_time) {
        return std::shared_ptr<fx>(new flash_when_hurt(flag, cooldown_time));
    }

}
