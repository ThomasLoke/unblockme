#include "stdafx.h"

#include "rinfo_hash.h"
using namespace std;

rinfo_hash::rinfo_hash(void) {}

rinfo_hash::rinfo_hash(const rinfo_hash& inst) {
	key = inst.key;
}

rinfo_hash::~rinfo_hash(void) {}

void rinfo_hash::clear(void) {
	key.clear();
}

void rinfo_hash::getkey(vector<rect>& rinfo) {
	// Check if key is allocated to the right dimensions (not that keys from different y-dimensions can have equal values - MEH SHOULD NOT BE A PROBLEM)
	if (key.size() != rinfo.size()) {
		setdim(rinfo.size());
	}
	// Compute key - take it to be either the x-coordinate ('h' type) or y-coordinate ('v' type), since the other coordinate should be immutable
	for (auto i = 0; i < rinfo.size(); ++i) {
		if (rinfo[i].type == 'h') {
			key[i] = rinfo[i].x;
		}
		else {
			key[i] = rinfo[i].y;
		}
	}
}

bool rinfo_hash::operator==(const rinfo_hash& inst) {
	// Check if key sizes match first
	if (key.size() != inst.key.size()) {
		return false;
	}
	// Element-by-element comparison (again, possible match with same values derived from different length bit sequences - but don't need to worry about that)
	for (auto i = 0; i < key.size(); ++i) {
		if (key[i] != inst.key[i]) {
			return false;
		}
	}
	return true;
}

void rinfo_hash::print(void) {
	printf("{");
	for (auto i = 0; i < key.size() - 1; ++i) {
		printf("%i,", key[i]);
	}
	printf("%i}", key.back());
}