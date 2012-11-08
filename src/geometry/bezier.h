#ifndef BEZIER_H
#define BEZIER_H

#include <stdint.h>

#include <vector>
using namespace std;

#include "types.h"

vector<point> bezier(const vector<point>& points, uint32_t num_midpoints);

#endif