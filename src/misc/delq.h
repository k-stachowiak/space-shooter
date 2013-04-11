#ifndef DELQ_H
#define DELQ_H

#include <vector>
#include <utility>

template<class T>
class del_queue {
        std::vector<std::pair<double, T>> _impl;
public:
        void push(T const& val, double delay) { _impl.emplace_back(delay, val); }
        void push(T const& val) { push(val, 0.0); }

        void consume(del_queue<T>& other) {
                _impl.insert(end(_impl),
                                begin(other._impl),
                                end(other._impl));
                other._impl.clear();
        }

        template<class Callback>
        void visit(double dt, Callback cb) {
                for(unsigned i = 0; i < _impl.size();) {
                        auto& pr = _impl[i];
                        if(pr.first > 0.0) {
                                pr.first -= dt;
                                ++i;
                        } else {
                                cb(pr.second);
                                _impl[i] = std::move(_impl.back());
                                _impl.pop_back();
                                // Note: no increment in this case.
                        }
                }
        }
};

#endif
