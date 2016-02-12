#include "stdafx.h"

#include "rect.h"
using namespace std;

rect::rect(void) {}

rect::rect(const rect& inst) {
	x = inst.x;
	y = inst.y;
	L = inst.L;
	type = inst.type;
}

rect::~rect(void) {}

void rect::clear(void) {
	x = 0;
	y = 0;
	L = 0;
	type = ' ';
}

void rect::setelem(ifstream& fsource) {
	// Note: Due to the difference in indexing from 1 (Mathematica) and 0 (C++), subtract 1 from xv and yv since Mathematica -> C++ in terms of convention
	fsource >> x >> y >> L >> type;
	--x; --y;
}

void rect::writelem(ofstream& foutput) {
	// Account for difference in indexing again...
	foutput << x + 1 << " " << y + 1 << " " << L << " " << type << endl;
}

void rect::print(void) {
	printf("{%i,%i,%i,%c}", x, y, L, type);
}