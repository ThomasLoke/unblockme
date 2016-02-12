#pragma once

#include "stdafx.h"

#include "rmove.h"

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

struct edgeinfo;
class boost_obj;

// Typedefs:
typedef boost::adjacency_list <boost::setS, boost::vecS, boost::directedS, boost::no_property, edgeinfo > graph_t;
typedef graph_t::vertex_descriptor vertex_descriptor;

// Structure for bundled edge properties
struct edgeinfo {
	// Data
	int weight;
	rmove mv;
};

class boost_obj {
public:
	// Data:
	graph_t g; // Graph
	int source = 0; // Source node
	std::vector<rmove> mvpath; // Stores a path from source to destination
	// Functions:
	// Empty constructor:
	boost_obj(void);
	// Copy constructor:
	boost_obj(const boost_obj& inst);
	// Empty destructor:
	~boost_obj(void);
	// clear: Empties the object
	void clear(void);
	// print: Prints graph properties
	void print(void);
	// g_add_edge: Adds edges going both directions to the graph (NOTE: mv is copied, so it can be changed as required)
	bool g_add_edge(int idx_l, int idx_r, rmove mv);
	// g_add_edge_single: Adds edge going l -> r only
	bool g_add_edge_single(int idx_l, int idx_r, rmove mv);
	// find_shortest_path: Finds the shortest path given the list of solution nodes based on the current graph - returns solution index from sol_idxs that has the shortest path
	int find_shortest_path(const std::vector<int>& sol_idxs);
};