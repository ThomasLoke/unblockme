#include "stdafx.h"

#include "unblockme_opt.h"
using namespace std;

unblockme_opt::unblockme_opt(void) : unblockme_base() {}

unblockme_opt::unblockme_opt(const unblockme_opt& inst) : unblockme_base(inst) {
	mv_list = inst.mv_list;
	hash_list = inst.hash_list;
}

unblockme_opt::~unblockme_opt(void) {}

void unblockme_opt::clear(void) {
	unblockme_base::clear();
	rinfo_0.clear();
	occmat_0.clear();
	mv_list.clear();
	hash_list.clear();
	sol_idxs.clear();
}

void unblockme_opt::read(const char *filename) {
	// Call overridden read function from base class
	unblockme_base::read(filename);
	// Initialize rinfo_0 and occmat_0 accordingly
	rinfo_0 = rinfo;
	occmat_0 = occmat;
}

void unblockme_opt::write(const char *filename) {
	ofstream foutput;
	int i;
	// Open output file to write
	foutput.open(filename, ofstream::out);
	if (!foutput.is_open()) {
		printf("Could not open specified file %s to write - terminating program", filename);
		exit(EXIT_FAILURE);
	}
	// Taking into account C++ to Mathematica indexing
	foutput << gsx << " " << gsy << " " << nrect << " " << sidx + 1 << " " << sdir << " " << bgraph.mvpath.size() << endl;
	for (i = 0; i < bgraph.mvpath.size(); ++i) {
		bgraph.mvpath[i].writelem(foutput);
	}
	foutput.close();
}

int unblockme_opt::match_hash(const rinfo_hash& hashref) {
	int i;
	for (i = 0; i < hash_list.size(); ++i) {
		if (hash_list[i] == hashref) {
			break;
		}
	}
	return i;
}

int unblockme_opt::match_hash_alloc(const rinfo_hash& hashref) {
	int ret = match_hash(hashref);
	if (ret == hash_list.size()) {
		hash_list.push_back(hashref);
	}
	return ret;
}

void unblockme_opt::build_hashes(void) {
	int i, prev, cur;
	bool reversible;
	rinfo_hash hashval;
	// Compute key of initial state and store it (builds on existing hashval, no need to empty it)
	hashval.getkey(rinfo_0);
	cur = match_hash_alloc(hashval);
	// Take initial state as the source node in the graph
	bgraph.source = cur;
	// Set current state to initial state
	rinfo = rinfo_0;
	occmat = occmat_0;
	prev = cur;
	for (i = 0; i < mv_list.size(); ++i) {
		// Execute move
		rect_move(mv_list[i]);
		// Compute corresponding hash value
		hashval.getkey(rinfo);
		// Get index for the current hash value (whether new or old) - automatically executes push_back() for hash value to hash_list if needed
		cur = match_hash_alloc(hashval);
		// TEDIOUS: Need to check if B -> A (given A -> B) is valid - if blank space exists opposite to the direction of the move, invalid
		//          Keep in mind that the move has already been executed (also calcspace > 0 always)
		reversible = true;
		if (rinfo[mv_list[i].ridx].type == 'h') {
			// If moved to the right, check space on the left
			if (mv_list[i].mx > 0) {
				// Check if space to the left is equal to the magnitude of the move to the right - if it does, its reversible
				reversible = (occmat.calcspace(rinfo[mv_list[i].ridx].x, rinfo[mv_list[i].ridx].y, 'l') == mv_list[i].mx);
			}
			// If moved to the left, check space on the right
			else {
				// Check if space to the right is equal to the magnitude of the move to the left - if it does, its reversible
				reversible = (occmat.calcspace(rinfo[mv_list[i].ridx].x + rinfo[mv_list[i].ridx].L - 1, rinfo[mv_list[i].ridx].y, 'r') == -mv_list[i].mx);
			}
		}
		else {
			// If moved up, check space on the bottom
			if (mv_list[i].my > 0) {
				// Check if space to the bottom is equal to the magnitude of the move up - if it does, its reversible
				reversible = (occmat.calcspace(rinfo[mv_list[i].ridx].x, rinfo[mv_list[i].ridx].y, 'd') == mv_list[i].my);
			}
			// If moved down, check space on the top
			else {
				// Check if space to the top is equal to the magnitude of the move down - if it does, its reversible
				reversible = (occmat.calcspace(rinfo[mv_list[i].ridx].x, rinfo[mv_list[i].ridx].y + rinfo[mv_list[i].ridx].L - 1, 'u') == -mv_list[i].my);
			}
		}
		// Attempt to add corresponding edge to graph (return value not used for now)
		if (reversible) {
			bgraph.g_add_edge(prev, cur, mv_list[i]);
		}
		else {
			bgraph.g_add_edge_single(prev, cur, mv_list[i]);
		}
		// Check if current state is a (new) solution - if it is, then store it
		if (is_solution() && cur == hash_list.size() - 1) {
			sol_idxs.push_back(cur);
		}
		// Update prev index to cur index
		prev = cur;
	}
}

void unblockme_opt::find_shortest_path(void) {
	if (sol_idxs.size() == 0) {
		printf("No solution vertices currently specified - exiting find_shortest_path()\n");
		return;
	}
	else {
		printf("Number of solutions: %i\n", sol_idxs.size());
	}
	bgraph.find_shortest_path(sol_idxs);
	bgraph.print();
}