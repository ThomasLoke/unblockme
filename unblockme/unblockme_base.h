#pragma once

#include "stdafx.h"

#include "rmove.h"
#include "occupancymatrix.h"

class unblockme_base {
	// Base class for unblockme_sys and unblockme_opt
public:
	// Data:
	int gsx = 0;
	int gsy = 0;
	int nrect = 0;
	std::vector<rect> rinfo;
	occupancymatrix occmat;
	// Target rectangle:
	int sidx, sdir;
	// Functions:
	// Empty constructor:
	unblockme_base(void);
	// Copy constructor:
	unblockme_base(const unblockme_base& inst);
	// Destructor:
	~unblockme_base(void);
	// clear: Empties the object
	void clear(void);
	// read: Reads the initial state of the system from a specified file
	void read(const char *filename);
	// is_solution: Checks if the current state of the system is a solution
	bool is_solution(void);
	// rect_move: Executes a given move, updating rinfo and occmat
	void rect_move(const rmove& mv);
	// rect_unmove: Un-executes a given move, updating rinfo and occmat
	void rect_unmove(const rmove& mv);
};