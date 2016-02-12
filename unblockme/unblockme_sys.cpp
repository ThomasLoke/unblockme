#include "stdafx.h"

#include "unblockme_sys.h"
using namespace std;

unblockme_sys::unblockme_sys(void) : unblockme_base() {}

unblockme_sys::unblockme_sys(const unblockme_sys& inst) : unblockme_base(inst) {
	sol_mvs = inst.sol_mvs;
	np = inst.np;
}

unblockme_sys::~unblockme_sys(void) {}

void unblockme_sys::clear(void) {
	unblockme_base::clear();
	sol_mvs.clear();
	np = 0;
}

void unblockme_sys::write(const char *filename) {
	ofstream foutput;
	int i;
	// Open output file to write
	foutput.open(filename, ofstream::out);
	if (!foutput.is_open()) {
		printf("Could not open specified file %s to write - terminating program", filename);
		exit(EXIT_FAILURE);
	}
	// Taking into account C++ to Mathematica indexing
	foutput << gsx << " " << gsy << " " << nrect << " " << sidx + 1 << " " << sdir << " " << np << endl;
	for (i = 0; i < nrect; ++i) {
		rinfo[i].writelem(foutput);
	}
	for (i = 0; i < sol_mvs.size(); ++i) {
		sol_mvs[i].writelem(foutput);
	}
	foutput.close();
}

void unblockme_sys::find_moves(vector<rmove>& mvs) {
	int i, spc;
	rmove temp;
	if (mvs.size() != 0) {
		mvs.clear();
	}
	for (i = 0; i < nrect; ++i) {
		switch (rinfo[i].type) {
		case 'h':
			// Horizontal block: Fixes y, varies x
			// Calculate space on the left (if not on boundary)
			if (rinfo[i].x > 0) {
				spc = occmat.calcspace(rinfo[i].x, rinfo[i].y, 'l');
				if (spc > 0) {
					temp.setelem(i, -spc, 0);
					mvs.push_back(temp);
				}
			}
			// Calculate space on the right (if not on boundary)
			if (rinfo[i].x < gsx - 1) {
				spc = occmat.calcspace(rinfo[i].x + rinfo[i].L - 1, rinfo[i].y, 'r');
				if (spc > 0) {
					temp.setelem(i, spc, 0);
					mvs.push_back(temp);
				}
			}
			break;
		case 'v':
			// Vertical block: Fixes x, varies y
			// Calculate space on the bottom (if not on boundary)
			if (rinfo[i].y > 0) {
				spc = occmat.calcspace(rinfo[i].x, rinfo[i].y, 'd');
				if (spc > 0) {
					temp.setelem(i, 0, -spc);
					mvs.push_back(temp);
				}
			}
			// Calculate space on the top (if not on boundary)
			if (rinfo[i].y < gsy - 1) {
				spc = occmat.calcspace(rinfo[i].x, rinfo[i].y + rinfo[i].L - 1, 'u');
				if (spc > 0) {
					temp.setelem(i, 0, spc);
					mvs.push_back(temp);
				}
			}
			break;
		default:
			break;
		}
	}
}

bool unblockme_sys::find_solution_bf(int max_depth) {
	// Main content here, calls find_solution_bf_rec
	np = 0;
	return find_solution_bf_rec(max_depth);
}

bool unblockme_sys::find_solution_bf_rec(int depth) {
	++np;
	// Check if current state is a solution - otherwise continue
	if (is_solution()) {
		return true;
	}
	// Check current depth - if still more than 0, continue
	if (depth == 0) {
		return false;
	}
	// Calculate possible moves
	vector<rmove> curmvs; // TODO: Look into optimising this if necessary - this causes at least ~3.5x slowness
	find_moves(curmvs);
	if (curmvs.size() > 0) {
		int i;
		// Remove undesirable moves
		// Case 1: (immediate) backward move
		if (sol_mvs.size() > 0) {
			for (i = 0; i < curmvs.size(); ++i) {
				if (curmvs[i].ridx == sol_mvs.back().ridx) {
					if (curmvs[i].mx == -sol_mvs.back().mx && curmvs[i].my == -sol_mvs.back().my) {
						break;
					}
				}
			}
		}
		// Found match for case 1
		if (i < curmvs.size()) {
			// Erase matched element
			curmvs.erase(curmvs.begin() + i);
		}
		// Loop through the new set of moves after erasing
		for (i = 0; i < curmvs.size(); ++i) {
			// Execute new move
			rect_move(curmvs[i]);
			// Add new move onto current solution
			sol_mvs.push_back(curmvs[i]);
			// Run recursive function with one less depth
			if (find_solution_bf_rec(depth - 1)) {
				// If solution found, we do...
				return true;
			}
			// Un-execute move
			rect_unmove(curmvs[i]);
			// Delete previous move
			sol_mvs.pop_back();
		}
	}
	return false;
}

bool unblockme_sys::find_solution_mc(int max_depth) {
	// Main content here, calls find_solution_mc_rec
	np = 0;
	vector<rmove> curmvs;
	minstd_rand gen((unsigned int)chrono::system_clock::now().time_since_epoch().count());
	return find_solution_mc_rec(max_depth, curmvs, gen);
}

bool unblockme_sys::find_solution_mc_rec(int depth, vector<rmove>& curmvs, minstd_rand& gen) {
	++np;
	/*
	// Check if current state is a solution - otherwise continue
	if (is_solution()) {
	return true;
	}
	*/
	// Check current depth - if still more than 0, continue
	if (depth == 0) {
		return false;
	}
	// Calculate possible moves
	find_moves(curmvs);
	if (curmvs.size() > 0) {
		int i;
		// Remove undesirable moves
		// Case 1: (immediate) backward move
		if (sol_mvs.size() > 0) {
			for (i = 0; i < curmvs.size(); ++i) {
				if (curmvs[i].ridx == sol_mvs.back().ridx) {
					if (curmvs[i].mx == -sol_mvs.back().mx && curmvs[i].my == -sol_mvs.back().my) {
						break;
					}
				}
			}
		}
		// Found match for case 1
		if (i < curmvs.size()) {
			// Erase matched element
			curmvs.erase(curmvs.begin() + i);
		}
		// Select random element
		i = gen() % curmvs.size();
		// Execute new move
		rect_move(curmvs[i]);
		// Add new move onto current solution
		sol_mvs.push_back(curmvs[i]);
		// Run recursive function with one less depth
		if (find_solution_mc_rec(depth - 1, curmvs, gen)) {
			// If solution found, we do...
			return true;
		}
		// No need to undo stuff - since well, its only going one way....
	}
	return false;
}