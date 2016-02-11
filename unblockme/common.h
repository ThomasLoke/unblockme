#pragma once

#include "stdafx.h"
using namespace std;

class rmove {
public:
	// Data:
	int ridx = 0;
	int mx = 0;
	int my = 0;
	// Functions:
	// Empty constructor:
	rmove(void) {}
	// Copy constructor:
	rmove(const rmove& inst) {
		ridx = inst.ridx;
		mx = inst.mx;
		my = inst.my;
	}
	// Destructor:
	~rmove(void) {}
	// clear: Empties the object
	void clear(void) {
		ridx = 0;
		mx = 0;
		my = 0;
	}
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
	void writelem(ofstream& foutput) {
		// Account for difference in indexing again...
		foutput << ridx + 1 << " " << mx << " " << my << endl;
	}
	// print: Print object in {ridx,mx,my} format
	void print(void) {
		printf("{%i,%i,%i}\n", ridx, mx, my);
	}
};