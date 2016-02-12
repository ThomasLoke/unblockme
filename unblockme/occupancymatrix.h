#pragma once

#include "stdafx.h"

#include "rect.h"

class occupancymatrix {
public:
	// Data:
	int gsx = 0;
	int gsy = 0;
	std::vector< std::vector<bool> > M;
	// Functions:
	// Empty constructor:
	occupancymatrix(void);
	// Copy constructor:
	occupancymatrix(const occupancymatrix& inst);
	// Constructor with size:
	occupancymatrix(int gsxv, int gsyv);
	// Destructor:
	~occupancymatrix(void);
	// clear: Empties the object
	void clear(void);
	// zero: Sets the content of M to false
	void zero(void);
	// setdim: Sets the dimension of the matrix, resizing matrix accordingly
	void setdim(int gsxv, int gsyv);
	// empty: Empties the matrix
	void empty(void);
	// fill: Fills the matrix according to a list of rectangles
	void fill(const std::vector<rect>& rinfo);
	// add: Fills the matrix according to a single rectangle
	void add(const rect& rinst);
	// remove: Fills the matrix according to a single rectangle
	void remove(const rect& rinst);
	// print: Print object in 0,1-grid format
	void print(void);
	// calcspace: Calculates the empty space from (but not including) a point {x,y} toward a certain direction (up until boundaries)
	// Format for dir - {'l' - left, 'r' - right, 'd' - down, 'u' - up}
	int calcspace(int x, int y, const char dir);
};