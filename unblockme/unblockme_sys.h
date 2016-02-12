#pragma once

#include "stdafx.h"

#include "unblockme_base.h"

class unblockme_sys : public unblockme_base {
public:
	// --Inherited data and functions from unblockme_base--
	// Solution info
	std::vector<rmove> sol_mvs;
	int np = 0; // Counter for number of recursive function calls
	// Functions:
	// Empty constructor:
	unblockme_sys(void);
	// Copy constructor:
	unblockme_sys(const unblockme_sys& inst);
	// Destructor:
	~unblockme_sys(void);
	// clear: Empties the object
	void clear(void);
	// write: Writes the current state of the system to a specified file
	void write(const char *filename);
	// find_moves: Finds all possible moves with the current system state, returns result in provided vector<move>
	void find_moves(std::vector<rmove>& mvs);
	// find_solution_bf: Finds solution using brute force
	bool find_solution_bf(int max_depth);
	// find_solution_bf_rec: Finds solution using brute force (main recursive part)
	bool find_solution_bf_rec(int depth);
	// find_solution_mc: Finds solution using Monte Carlo
	bool find_solution_mc(int max_depth);
	// find_solution_mc_rec: Finds solution using Monte Carlo (main recursive part)
	bool find_solution_mc_rec(int depth, std::vector<rmove>& curmvs, std::minstd_rand& gen);
};