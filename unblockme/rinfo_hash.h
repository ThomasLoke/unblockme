#pragma once

#include "stdafx.h"

#include "rect.h"

class rinfo_hash {
public:
	// Data:
	std::vector<int> key;
	// Functions:
	// Empty constructor:
	rinfo_hash(void);
	// Copy constructor:
	rinfo_hash(const rinfo_hash& inst);
	// Empty destructor:
	~rinfo_hash(void);
	// clear: Empties the object
	void clear(void);
	// setdim: Sets the dimension of the object, preallocates the key
	void inline setdim(int dim) {
		key.resize(dim, 0);
	}
	// getkey: Computes the key from a list of rectangles
	void getkey(std::vector<rect>& rinfo);
	// operator == : returns true if two keys are the same
	bool operator==(const rinfo_hash& inst);
	// print: Print object in format {--list of hash values--}
	void print(void);
};
