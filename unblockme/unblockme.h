#pragma once

#include "stdafx.h"
#include "common.h"
#include "boost_obj.h"
using namespace std;

class rect;
class occupancymatrix;
class occupancymatrix_hash;
class unblockme;
class unblockme_opt;

class rect {
public:
	// Data:
	int x = 0;
	int y = 0;
	int L = 0;
	char type = ' ';
	// Functions:
	// Empty constructor:
	rect(void) {}
	// Copy constructor:
	rect(const rect& inst) {
		x = inst.x;
		y = inst.y;
		L = inst.L;
		type = inst.type;
	}
	// Destructor:
	~rect(void) {}
	// clear: Empties the object
	void clear(void) {
		x = 0;
		y = 0;
		L = 0;
		type = ' ';
	}
	// updatexy: Update x,y coordinates of rectangle
	void inline updatexy(int xv, int yv) {
		x = xv;
		y = yv;
	}
	// move: Shift x,y coordinates of rectangle by some specified amount
	void inline move(int mvx, int mvy) {
		x += mvx;
		y += mvy;
	}
	// setelem: Sets the value of all data elements of rectangle
	void inline setelem(int xv, int yv, int Lv, char typev) {
		updatexy(xv, yv);
		L = Lv;
		type = typev;
	}
	// setelem: Set object contents from file
	void setelem(ifstream& fsource) {
		// Note: Due to the difference in indexing from 1 (Mathematica) and 0 (C++), subtract 1 from xv and yv since Mathematica -> C++ in terms of convention
		fsource >> x >> y >> L >> type;
		--x; --y;
	}
	// writelem: Write object contents to file
	void writelem(ofstream& foutput) {
		// Account for difference in indexing again...
		foutput << x + 1 << " " << y + 1 << " " << L << " " << type << endl;
	}
	// print: Print object in {x,y,L,type} format
	void print(void) {
		printf("{%i,%i,%i,%c}", x, y, L, type);
	}
};

class occupancymatrix {
public:
	// Data:
	int gsx = 0;
	int gsy = 0;
	vector< vector<bool> > M;
	// Functions:
	// Empty constructor:
	occupancymatrix(void) {}
	// Copy constructor:
	occupancymatrix(const occupancymatrix& inst) {
		if (gsx != inst.gsx || gsy != inst.gsy) {
			gsx = inst.gsx;
			gsy = inst.gsy;
		}
		M = inst.M; // Check that this works at all levels
	}
	// Constructor with size:
	occupancymatrix(int gsxv, int gsyv) {
		setdim(gsxv, gsyv);
	}
	// Destructor:
	~occupancymatrix(void) {}
	// clear: Empties the object
	void clear(void) {
		gsx = 0;
		gsy = 0;
		M.clear();
	}
	// zero: Sets the content of M to false
	void zero(void) {
		int i, j;
		for (i = 0; i < gsx; ++i) {
			for (j = 0; j < gsy; ++j) {
				M[i][j] = false;
			}
		}
	}
	// setdim: Sets the dimension of the matrix, resizing matrix accordingly
	void setdim(int gsxv, int gsyv) {
		int i;
		gsx = gsxv;
		gsy = gsyv;
		M.resize(gsxv);
		for (i = 0;i < gsyv; ++i) {
			M[i].resize(gsy, false);
		}
	}
	// empty: Empties the matrix
	void empty(void) {
		int i, j;
		for (i = 0; i < gsx; ++i) {
			for (j = 0; j < gsy; ++j) {
				M[i][j] = false;
			}
		}
	}
	// fill: Fills the matrix according to a list of rectangles
	void fill(const vector<rect>& rinfo) {
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
	// add: Fills the matrix according to a single rectangle
	void add(const rect& rinst) {
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
	// remove: Fills the matrix according to a single rectangle
	void remove(const rect& rinst) {
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
	// print: Print object in 0,1-grid format
	void print(void) {
		int i, j;
		for (i = 0; i < gsx; ++i) {
			for (j = 0; j < gsy - 1; ++j) {
				printf("%i ", (int)M[i][j]);
			}
			printf("%i\n", (int)M[i][j]);
		}
	}
	// calcspace: Calculates the empty space from (but not including) a point {x,y} toward a certain direction (up until boundaries)
	// Format for dir - {'l' - left, 'r' - right, 'd' - down, 'u' - up}
	int calcspace(int x, int y, const char dir) {
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
};

class rinfo_hash {
public:
	// Data:
	vector<int> key;
	// Functions:
	// Empty constructor:
	rinfo_hash(void) {}
	// Copy constructor:
	rinfo_hash(const rinfo_hash& inst) {
		key = inst.key;
	}
	// Empty destructor:
	~rinfo_hash(void) {}
	// clear: Empties the object
	void clear(void) {
		key.clear();
	}
	// setdim: Sets the dimension of the object, preallocates the key
	void inline setdim(int dim) {
		key.resize(dim, 0);
	}
	// getkey: Computes the key from a list of rectangles
	void getkey(vector<rect>& rinfo) {
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
	// operator == : returns true if two keys are the same
	bool operator==(const rinfo_hash& inst) {
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
	// print: Print object in format {--list of hash values--}
	void print(void) {
		printf("{");
		for (auto i = 0; i < key.size() - 1; ++i) {
			printf("%i,", key[i]);
		}
		printf("%i}", key.back());
	}
};

class unblockme_base {
	// Base class for unblockme_sys and unblockme_opt
public:
	// Data:
	int gsx = 0;
	int gsy = 0;
	int nrect = 0;
	vector<rect> rinfo;
	occupancymatrix occmat;
	// Target rectangle:
	int sidx, sdir;
	// Functions:
	// Empty constructor:
	unblockme_base(void) {}
	// Copy constructor:
	unblockme_base(const unblockme_base& inst) {
		gsx = inst.gsx;
		gsy = inst.gsy;
		nrect = inst.nrect;
		rinfo = inst.rinfo;
		occmat = inst.occmat;
		sidx = inst.sidx;
		sdir = inst.sdir;
	}
	// Destructor:
	~unblockme_base(void) {}
	// clear: Empties the object
	void clear(void) {
		gsx = 0;
		gsy = 0;
		nrect = 0;
		rinfo.clear();
		occmat.clear();
	}
	// read: Reads the initial state of the system from a specified file
	void read(const char *filename) {
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
	// is_solution: Checks if the current state of the system is a solution
	bool is_solution(void) {
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
	// rect_move: Executes a given move, updating rinfo and occmat
	void rect_move(const rmove& mv) {
		// Remove old rectangle from occmat
		occmat.remove(rinfo[mv.ridx]);
		// Update rectangle
		rinfo[mv.ridx].move(mv.mx, mv.my);
		// Add new rectangle to occmat
		occmat.add(rinfo[mv.ridx]);
	}
	// rect_unmove: Un-executes a given move, updating rinfo and occmat
	void rect_unmove(const rmove& mv) {
		// Remove old rectangle from occmat
		occmat.remove(rinfo[mv.ridx]);
		// Unupdate rectangle
		rinfo[mv.ridx].move(-mv.mx, -mv.my);
		// Add new rectangle to occmat
		occmat.add(rinfo[mv.ridx]);
	}
};

class unblockme_sys : public unblockme_base {
public:
	// --Inherited data and functions from unblockme_base--
	// Solution info
	vector<rmove> sol_mvs;
	int np = 0; // Counter for number of recursive function calls
	// Functions:
	// Empty constructor:
	unblockme_sys(void) : unblockme_base() {}
	// Copy constructor:
	unblockme_sys(const unblockme_sys& inst) : unblockme_base(inst) {
		sol_mvs = inst.sol_mvs;
		np = inst.np;
	}
	// Destructor:
	~unblockme_sys(void) {}
	// clear: Empties the object
	void clear(void) {
		unblockme_base::clear();
		sol_mvs.clear();
		np = 0;
	}
	// write: Writes the current state of the system to a specified file
	void write(const char *filename) {
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
	// find_moves: Finds all possible moves with the current system state, returns result in provided vector<move>
	void find_moves(vector<rmove>& mvs) {
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
	// find_solution_bf: Finds solution using brute force
	bool find_solution_bf(int max_depth) {
		// Main content here, calls find_solution_bf_rec
		np = 0;
		return find_solution_bf_rec(max_depth);
	}
	// find_solution_bf_rec: Finds solution using brute force (main recursive part)
	bool find_solution_bf_rec(int depth) {
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
	// find_solution_mc: Finds solution using Monte Carlo
	bool find_solution_mc(int max_depth) {
		// Main content here, calls find_solution_mc_rec
		np = 0;
		vector<rmove> curmvs;
		minstd_rand gen((unsigned int)chrono::system_clock::now().time_since_epoch().count());
		return find_solution_mc_rec(max_depth, curmvs, gen);
	}
	// find_solution_mc_rec: Finds solution using Monte Carlo (main recursive part)
	bool find_solution_mc_rec(int depth, vector<rmove>& curmvs, minstd_rand& gen) {
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
};

class unblockme_opt : public unblockme_base {
public:
	// --Inherited data and functions from unblockme_base--
	// Initial state copies
	vector<rect> rinfo_0;
	occupancymatrix occmat_0;
	// Some list of moves (last element may or may not be a solution - recomputed)
	vector<rmove> mv_list; // Consider putting this outside the class - retained for now
	vector<rinfo_hash> hash_list;
	vector<int> sol_idxs; // Index of solutions
	// Graph info
	boost_obj bgraph;
	// Functions:
	// Empty constructor:
	unblockme_opt(void) : unblockme_base() {}
	// Copy constructor:
	unblockme_opt(const unblockme_opt& inst) : unblockme_base(inst) {
		mv_list = inst.mv_list;
		hash_list = inst.hash_list;
	}
	// Destructor:
	~unblockme_opt(void) {}
	// clear: Empties the object
	void clear(void) {
		unblockme_base::clear();
		rinfo_0.clear();
		occmat_0.clear();
		mv_list.clear();
		hash_list.clear();
		sol_idxs.clear();
	}
	// read (override): Does the same as unblockme_base & initializes rinfo_0 and occmat_0
	void read(const char *filename) {
		// Call overridden read function from base class
		unblockme_base::read(filename);
		// Initialize rinfo_0 and occmat_0 accordingly
		rinfo_0 = rinfo;
		occmat_0 = occmat;
	}
	// write: Writes the current state of the system to a specified file
	void write(const char *filename) {
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
	// match_hash: Matches given hash to an element of hash_list, returning the corresponding index between [0,hash_list.size()) - if no match found, returns hash_list.size()
	int match_hash(const rinfo_hash& hashref) {
		int i;
		for (i = 0; i < hash_list.size(); ++i) {
			if (hash_list[i] == hashref) {
				break;
			}
		}
		return i;
	}
	// match_hash_alloc: Same as match_hash, except if no match is found, stores the given hash in hash_list as well
	int match_hash_alloc(const rinfo_hash& hashref) {
		int ret = match_hash(hashref);
		if (ret == hash_list.size()) {
			hash_list.push_back(hashref);
		}
		return ret;
	}
	// build_hashes: Traverses through list of moves and computes the hash value for each, including the initial state - adds relevant information to bgraph
	void build_hashes(void) {
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
			if (is_solution() && cur == hash_list.size() - 1 ) {
				sol_idxs.push_back(cur);
			}
			// Update prev index to cur index
			prev = cur;
		}
	}
	// find_shortest_path: Finds the shortest path given the current graph state
	void find_shortest_path(void) {
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
};