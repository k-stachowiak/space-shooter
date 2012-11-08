#ifndef FRAME_DEF_H
#define FRAME_DEF_H

struct frame_def {

	uint32_t index;
	double time;

	// For easy initialization.
	frame_def(uint32_t index, double time)
		: index(index)
		, time(time) {
	}

	// For storing in the array :[
	frame_def() 
		: index(-1)
		, time(-1) {
	}

	frame_def& operator=(const frame_def& rhs) {
		index = rhs.index;
		time = rhs.time;
		return *this;
	}
};

#endif