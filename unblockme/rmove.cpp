#include "stdafx.h"

#include "rmove.h"
using namespace std;

rmove::rmove(void) {}

rmove::rmove(const rmove& inst) {
	ridx = inst.ridx;
	mx = inst.mx;
	my = inst.my;
}

rmove::~rmove(void) {}

void rmove::clear(void) {
	ridx = 0;
	mx = 0;
	my = 0;
}

void rmove::writelem(ofstream& foutput) {
	// Account for difference in indexing again...
	foutput << ridx + 1 << " " << mx << " " << my << endl;
}

void rmove::print(void) {
	printf("{%i,%i,%i}\n", ridx, mx, my);
}