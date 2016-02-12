#include "stdafx.h"

#include "unblockme_base.h"
using namespace std;

unblockme_base::unblockme_base(void) {}

unblockme_base::unblockme_base(const unblockme_base& inst) {
	gsx = inst.gsx;
	gsy = inst.gsy;
	nrect = inst.nrect;
	rinfo = inst.rinfo;
	occmat = inst.occmat;
	sidx = inst.sidx;
	sdir = inst.sdir;
}

unblockme_base::~unblockme_base(void) {}

void unblockme_base::clear(void) {
	gsx = 0;
	gsy = 0;
	nrect = 0;
	rinfo.clear();
	occmat.clear();
}

void unblockme_base::read(const char *filename) {
	ifstream fsource;
	rect rinst;
	int i;
	// Open file with problem description to read
	fsource.open(filename, ifstream::in);
	if (!fsource.is_open()) {
		printf("Could not open specified file %s to read - terminating program", filename);
		exit(EXIT_FAILURE);
	}
	fsource >> gsx >> gsy >> nrect >> sidx >> sdir;
	--sidx; // To change from Mathematica to C++ indexing (1 to 0)
	for (i = 0; i < nrect; ++i) {
		rinst.setelem(fsource);
		rinfo.push_back(rinst);
	}
	fsource.close();
	// Initialize occupancy matrix from {gsx,gsy} parameters and rinfo
	occmat.setdim(gsx, gsy);
	occmat.zero();
	occmat.fill(rinfo);
}

bool unblockme_base::is_solution(void) {
	switch (rinfo[sidx].type) {
	case 'h':
		if (sdir == -1) {
			return occmat.calcspace(rinfo[sidx].x, rinfo[sidx].y, 'l') == rinfo[sidx].x;
		}
		else {
			return occmat.calcspace(rinfo[sidx].x + rinfo[sidx].L - 1, rinfo[sidx].y, 'r') == gsx - rinfo[sidx].x - rinfo[sidx].L;
		}
		break;
	case 'v':
		if (sdir == -1) {
			return occmat.calcspace(rinfo[sidx].x, rinfo[sidx].y, 'd') == rinfo[sidx].y;
		}
		else {
			return occmat.calcspace(rinfo[sidx].x, rinfo[sidx].y + rinfo[sidx].L - 1, 'u') == gsy - rinfo[sidx].y - rinfo[sidx].L;
		}
		break;
	default:
		return true;
		break;
	}
}

void unblockme_base::rect_move(const rmove& mv) {
	// Remove old rectangle from occmat
	occmat.remove(rinfo[mv.ridx]);
	// Update rectangle
	rinfo[mv.ridx].move(mv.mx, mv.my);
	// Add new rectangle to occmat
	occmat.add(rinfo[mv.ridx]);
}

void unblockme_base::rect_unmove(const rmove& mv) {
	// Remove old rectangle from occmat
	occmat.remove(rinfo[mv.ridx]);
	// Unupdate rectangle
	rinfo[mv.ridx].move(-mv.mx, -mv.my);
	// Add new rectangle to occmat
	occmat.add(rinfo[mv.ridx]);
}