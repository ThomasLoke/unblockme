#include "stdafx.h"

#include "unblockme_sys.h"
#include "unblockme_opt.h"
using namespace std;

// Converts a given duration into seconds. Warning: This is a bittt unsafe, so be careful!
template <typename duration_type>
inline double dur_to_secs(const duration_type& dur) {
	return (chrono::duration_cast<chrono::nanoseconds>(dur)).count() / 1000000000.0;
}
// Calculates time difference from time points
template <typename clock_type>
inline double time_diff(const chrono::time_point<clock_type>& t_start, const chrono::time_point<clock_type>& t_end) {
	return dur_to_secs(t_end - t_start);
}

int main(int argc, char* argv[])
{
	/*
	Notes:
	-> In general, learning that the more efficient code is often the more verbose one, i.e. almost no reduction using intermediate variables (needs allocation & copying)
	-> is_solution() is pretty optimal - about at least 10 times faster than find_moves(), so not going to bottleneck
	-> find_moves() is a problem - when used in context of the recursive routine, declaring a new vector<move> for each is quite expensive - about 3.5 times slower. Specific numbers:
	--> For single vector<rmove> with emptying cost, iterated 10^6 times, 0.2357 sec
	--> For new vector<rmove> per each cycle of the loop, iterated 10^6 times, 0.9010 sec
	-> These numbers in and of themselves are quite worrying - supposing we have about 8 moves on average, then in 1 sec we can process at least ~7 levels.
	   However solution is usually > 10 moves down - what do?? Brute force is unlikely to be feasible - with full optimisation (+given a few mins) could 
	   probably reach ~11 levels, which is still not sufficient.
	-> Some numbers (with new vector<rmove> per each cycle of the loop):
	--> Can process ~3.92*10^6 (about 4 million) recursive function calls per second (which is at least ~1000 times better than Mathematica...)
	--> 11 levels: ~639 million recursive function calls, ~162 seconds, no solution found.
	-> Probably need a heuristic solution (aim: at least 20 levels?) - two routes:
	--> Prescribe some form of quality factor for solution - possibly by using overlap of occmat; or
	--> Monte-Carlo solution - i.e. just do a random walk. This result however is unlikely to be useful - at best requires some human post-processing. Promising though, since from
	    brute force numbers, we could simulate a chain of 10^6 moves easy in 1 sec.
	-> Monte-Carlo simulation results - tl;dr: IT WORKS!!!!
	--> Takes about 500-2000 moves to find solution - super fast.
	--> Still not a reasonable solution for a human - consider intermediate reductions by matching some (large) number of steps to an exact propagation of <= X moves.
	--> Idea: Subdivide the list of moves into blocks - close to end of block take samples, and use them as valid states to search for using exact propagation of <= moves.
	--> Or: Recursive reduction of a block of 2X (or 3X, 4X etc) moves into X? Not sure if this can reach close to the optimal solution though....
	--> Also: Better elimination routines?
	--> Another idea: Hash keys for occupancy matrix using bit string -> integer for each row
	--> Markov chain property: Doesn't matter how you get the current state - its equivalent to a past state that has the same configuration. Can use this to match and cut down
	--> A bit tricky however - the sequence ABCCCCACCBD can be reduced to ACCBD (ACBD) or ABD or ABCBD (ABD) - ABD is the most optimal, but can only find it by going through all (n(n+1)/2) 
	    possibilities (each time)? Interval type [A,M1) [M2,D] - avoids reallocation?
	--> New idea - build adjacency relations from sequence and use Dijkstra's algorithm to compute the shortest path (A to D in the above sequence). Requires use of BOOST library.
	    Some technical details:
	---> Define a vertex v_i to be a unique state. A unique state (for the purposes of this algorithm) is defined by the hash value of the occupancy matrix. The hash function is a 1-to-1
	     function given an occupancy matrix - however, is the map vector<rect> -> occupancy matrix 1-to-1? Assuming no blocks that can move in all four directions (i.e. a unit square) 
		 it should be, since there should be no way to reshuffle the order of rectangles in any particular row/column to achieve the same occupancy matrix.
	---> An edge {v_i,v_j} defines a unique move that takes the vector<rect> implicit in v_i to the vector<rect> implicit in v_j. If this move is {ridx,mx,my}, then note that we can always
	     define the edge {v_j,v_i} with the inverse move {ridx,-mx,-my}, i.e. the graph is always undirected.
	---> This suggests an intriguing idea - instead of terminating a sequence (generated by a random walk) after finding the solution, we can just propagate the sequence indefinitely and
	     then build up the adjacency relations, from which a more 'complete' graph (which is also typically larger) can be built (and so, higher chance of finding a shorter path that is
		 at worst the same as terminating at the first solution). However, we'd also have to keep track of the multiple solution vertices (still just one initial vertex), so we'd have to 
		 find the shortest path to each solution vertex, and then take the solution vertex which gives a shorter path from the initial vertex.
	---> !!!!!! Learnt the REALLY hard way that the occupancy matrix (and by extension, the integer hash) is NOT 1-to-1 - use list of coordinates for rectangles instead 
	---> Majority of time spent in building hash_list - takes a quadratic amount of time since need to traverse hash_list every time we try to match a new state
	*/
	unblockme_sys usys;
	unblockme_opt uopt;
	auto t_start = chrono::high_resolution_clock::now();
	auto t_end = chrono::high_resolution_clock::now();
	auto td_1 = 0*(t_end - t_start);
	auto td_2 = td_1;
	auto td_temp = td_1;
	int ntrials = 5;
	// Read in problem description for the optimiser (only need to run once)
	uopt.read("problemdef.dat");
	for (auto trial = 0; trial < ntrials; ++trial) {
		printf("-----Trial #%i-----\n", trial+1);
		// Initialize the solution finder
		usys.clear();
		usys.read("problemdef.dat");
		// Run solution finder
		t_start = chrono::high_resolution_clock::now();
		bool temp = usys.find_solution_mc(10000);
		t_end = chrono::high_resolution_clock::now();
		td_temp = t_end - t_start;
		td_1 += td_temp;
		printf("Time taken to find solution: %.9f seconds\n", dur_to_secs(td_temp));
		// Run optimiser
		t_start = chrono::high_resolution_clock::now();
		uopt.mv_list = usys.sol_mvs;
		uopt.build_hashes();
		uopt.find_shortest_path();
		t_end = chrono::high_resolution_clock::now();
		td_temp = t_end - t_start;
		td_2 += td_temp;
		printf("Time taken to optimise solution: %.9f seconds\n", dur_to_secs(td_temp));
		printf("\n");
	}
	// Output results
	printf("Time taken in total: %.9f seconds\n", dur_to_secs(td_1 + td_2));
	uopt.write("problemsol.dat");
    return 0;
}