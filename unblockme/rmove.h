#pragma once

#include "stdafx.h"

class rmove {
public:
	// Data:
	int ridx = 0;
	int mx = 0;
	int my = 0;
	// Functions:
	// Empty constructor:
	rmove(void);
	// Copy constructor:
	rmove(const rmove& inst);
	// Destructor:
	~rmove(void);
	// clear: Empties the object
	void clear(void);
	// setelem: Sets the value of all data elements of rectangle
	void inline setelem(int ridxv, int mxv, int myv) {
		ridx = ridxv;
		mx = mxv;
		my = myv;
	}
	// reverse: Reverses the move
	void inline reverse(void) {
		mx = -mx;
		my = -my;
	}
	// writelem: Write object contents to file
	void writelem(std::ofstream& foutput);
	// print: Print object in {ridx,mx,my} format
	void print(void);
};