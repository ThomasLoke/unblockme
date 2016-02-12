#include "stdafx.h"

#include "occupancymatrix.h"
using namespace std;

occupancymatrix::occupancymatrix(void) {}

occupancymatrix::occupancymatrix(const occupancymatrix& inst) {
	if (gsx != inst.gsx || gsy != inst.gsy) {
		gsx = inst.gsx;
		gsy = inst.gsy;
	}
	M = inst.M; // Check that this works at all levels
}

occupancymatrix::occupancymatrix(int gsxv, int gsyv) {
	setdim(gsxv, gsyv);
}

occupancymatrix::~occupancymatrix(void) {}

void occupancymatrix::clear(void) {
	gsx = 0;
	gsy = 0;
	M.clear();
}

void occupancymatrix::zero(void) {
	int i, j;
	for (i = 0; i < gsx; ++i) {
		for (j = 0; j < gsy; ++j) {
			M[i][j] = false;
		}
	}
}

void occupancymatrix::setdim(int gsxv, int gsyv) {
	int i;
	gsx = gsxv;
	gsy = gsyv;
	M.resize(gsxv);
	for (i = 0;i < gsyv; ++i) {
		M[i].resize(gsy, false);
	}
}

void occupancymatrix::empty(void) {
	int i, j;
	for (i = 0; i < gsx; ++i) {
		for (j = 0; j < gsy; ++j) {
			M[i][j] = false;
		}
	}
}

void occupancymatrix::fill(const std::vector<rect>& rinfo) {
	int i, j;
	// Rough timing results indicate that saving to a temporary rect object is slower
	for (i = 0; i < rinfo.size(); ++i) {
		switch (rinfo[i].type) {
		case 'h':
			// Iterator traversal is slower compared to index traversal, so use indices
			for (j = rinfo[i].x; j < rinfo[i].x + rinfo[i].L; ++j) {
				M[j][rinfo[i].y] = true;
			}
			break;
		case 'v':
			for (j = rinfo[i].y; j < rinfo[i].y + rinfo[i].L; ++j) {
				M[rinfo[i].x][j] = true;
			}
			break;
		default:
			break;
		}
	}
}

void occupancymatrix::add(const rect& rinst) {
	int j;
	switch (rinst.type) {
	case 'h':
		for (j = rinst.x; j < rinst.x + rinst.L; ++j) {
			M[j][rinst.y] = true;
		}
		break;
	case 'v':
		for (j = rinst.y; j < rinst.y + rinst.L; ++j) {
			M[rinst.x][j] = true;
		}
		break;
	default:
		break;
	}
}

void occupancymatrix::remove(const rect& rinst) {
	int j;
	switch (rinst.type) {
	case 'h':
		for (j = rinst.x; j < rinst.x + rinst.L; ++j) {
			M[j][rinst.y] = false;
		}
		break;
	case 'v':
		for (j = rinst.y; j < rinst.y + rinst.L; ++j) {
			M[rinst.x][j] = false;
		}
		break;
	default:
		break;
	}
}

void occupancymatrix::print(void) {
	int i, j;
	for (i = 0; i < gsx; ++i) {
		for (j = 0; j < gsy - 1; ++j) {
			printf("%i ", (int)M[i][j]);
		}
		printf("%i\n", (int)M[i][j]);
	}
}

int occupancymatrix::calcspace(int x, int y, const char dir) {
	int i, space;
	// Can't use STL std::find since we can't specify an iterator over columns, since it's over objects
	switch (dir) {
	case 'l':
		// Checks blank space to the left of {x,y}
		for (i = x - 1; i >= 0; --i) {
			if (M[i][y]) {
				break;
			}
		}
		space = x - 1 - i;
		break;
	case 'r':
		// Checks blank space to the right of {x,y}
		for (i = x + 1; i < gsx; ++i) {
			if (M[i][y]) {
				break;
			}
		}
		space = i - x - 1;
		break;
	case 'd':
		// Checks blank space below {x,y}
		for (i = y - 1; i >= 0; --i) {
			if (M[x][i]) {
				break;
			}
		}
		space = y - 1 - i;
		break;
	case 'u':
		// Checks blank space above {x,y}
		for (i = y + 1; i < gsy; ++i) {
			if (M[x][i]) {
				break;
			}
		}
		space = i - y - 1;
		break;
	default:
		break;
	}
	return space;
}