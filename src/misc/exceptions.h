#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
using namespace std;

class initialization_error : public runtime_error {
public:
	initialization_error(string message)
	: runtime_error(message) {}
};

#endif