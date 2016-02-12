#pragma once

#include "stdafx.h"

class rect {
public:
	// Data:
	int x = 0;
	int y = 0;
	int L = 0;
	char type = ' ';
	// Functions:
	// Empty constructor:
	rect(void);
	// Copy constructor:
	rect(const rect& inst);
	// Destructor:
	~rect(void);
	// clear: Empties the object
	void clear(void);
	// updatexy: Update x,y coordinates of rectangle
	void inline updatexy(int xv, int yv) {
		x = xv;
		y = yv;
	}
	// move: Shift x,y coordinates of rectangle by some specified amount
	void inline move(int mvx, int mvy) {
		x += mvx;
		y += mvy;
	}
	// setelem: Sets the value of all data elements of rectangle
	void inline setelem(int xv, int yv, int Lv, char typev) {
		updatexy(xv, yv);
		L = Lv;
		type = typev;
	}
	// setelem: Set object contents from file
	void setelem(std::ifstream& fsource);
	// writelem: Write object contents to file
	void writelem(std::ofstream& foutput);
	// print: Print object in {x,y,L,type} format
	void print(void);
};