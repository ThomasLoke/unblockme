#pragma once

#include "stdafx.h"

#include "boost_obj.h"
#include "occupancymatrix.h"
#include "rinfo_hash.h"
#include "unblockme_base.h"

class unblockme_opt : public unblockme_base {
public:
	// --Inherited data and functions from unblockme_base--
	// Initial state copies
	std::vector<rect> rinfo_0;
	occupancymatrix occmat_0;
	// Some list of moves (last element may or may not be a solution - recomputed)
	std::vector<rmove> mv_list; // Consider putting this outside the class - retained for now
	std::vector<rinfo_hash> hash_list;
	std::vector<int> sol_idxs; // Index of solutions
	// Graph info
	boost_obj bgraph;
	// Functions:
	// Empty constructor:
	unblockme_opt(void);
	// Copy constructor:
	unblockme_opt(const unblockme_opt& inst);
	// Destructor:
	~unblockme_opt(void);
	// clear: Empties the object
	void clear(void);
	// read (override): Does the same as unblockme_base & initializes rinfo_0 and occmat_0
	void read(const char *filename);
	// write: Writes the current state of the system to a specified file
	void write(const char *filename);
	// match_hash: Matches given hash to an element of hash_list, returning the corresponding index between [0,hash_list.size()) - if no match found, returns hash_list.size()
	int match_hash(const rinfo_hash& hashref);
	// match_hash_alloc: Same as match_hash, except if no match is found, stores the given hash in hash_list as well
	int match_hash_alloc(const rinfo_hash& hashref);
	// build_hashes: Traverses through list of moves and computes the hash value for each, including the initial state - adds relevant information to bgraph
	void build_hashes(void);
	// find_shortest_path: Finds the shortest path given the current graph state
	void find_shortest_path(void);
};