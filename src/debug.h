#ifndef DEBUG_H
#define DEBUG_H

#ifdef _DEBUG

#include <iostream>
using std::cout;
using std::endl;

#define DEBUG_MSG(x) cout << x << endl;

#else

#define DEBUG_MSG(_)

#endif

#endif