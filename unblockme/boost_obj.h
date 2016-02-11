#pragma once

#include "stdafx.h"
#include "common.h"
using namespace std;

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
	vector<rmove> mvpath; // Stores a path from source to destination
	// Functions:
	// Empty constructor:
	boost_obj(void) {}
	// Copy constructor:
	boost_obj(const boost_obj& inst) {
		g = inst.g;
		source = inst.source;
		mvpath = inst.mvpath;
	}
	// Empty destructor:
	~boost_obj(void) {}
	// clear: Empties the object
	void clear(void) {
		g.clear();
		source = 0;
		mvpath.clear();
	}
	// print: Prints graph properties
	void print(void) {
		printf("Graph properties:\nNumber of vertices - %i\nNumber of directed edges - %i\n",boost::num_vertices(g),boost::num_edges(g));
		if (mvpath.size() > 0) {
			printf("Current solution path from source to destination uses %i moves\n",mvpath.size());
		}
	}
	// g_add_edge: Adds edges going both directions to the graph (NOTE: mv is copied, so it can be changed as required)
	bool g_add_edge(int idx_l, int idx_r, rmove mv) {
		bool res;
		// Adds the directed edge l-r with weight 1 and with the associated move
		res = boost::add_edge(idx_l, idx_r, { 1,mv }, g).second;
		if (!res) {
			//rmove temp = g[boost::edge(idx_l, idx_r, g).first].mv;
			//printf("Comparing edge conflict: Original move is {%i,%i,%i} and new move is {%i,%i,%i}\n", temp.ridx, temp.mx, temp.my, mv.ridx, mv.mx, mv.my);
			return false;
		}
		else {
			// Adds the directed edge r-l with weight 1 and with the associated reversed move
			mv.reverse();
			return boost::add_edge(idx_r, idx_l, { 1,mv }, g).second;
		}
	}
	// g_add_edge_single: Adds edge going l -> r only
	bool g_add_edge_single(int idx_l, int idx_r, rmove mv) {
		// Adds the directed edge l-r with weight 1 and with the associated move
		return boost::add_edge(idx_l, idx_r, { 1,mv }, g).second;
	}
	// find_shortest_path: Finds the shortest path given the list of solution nodes based on the current graph - returns solution index from sol_idxs that has the shortest path
	int find_shortest_path(const vector<int>& sol_idxs) {
		int i, ret;
		// Create predecessor map
		vector<vertex_descriptor> p(boost::num_vertices(g));
		// Create distance map (from source)
		vector<int> d(boost::num_vertices(g));
		// Create descriptor for the source node
		vertex_descriptor s = boost::vertex(source, g);
		// Evaluate Dijkstra on graph g with source s, predecessor_map p and distance_map d
		boost::dijkstra_shortest_paths(g, s, boost::weight_map(boost::get(&edgeinfo::weight, g)).predecessor_map(p.data()).distance_map(d.data()));
		// Out of all the solutions, pick the one with the minimum cost
		vertex_descriptor vidx;
		int cost;
		vidx = boost::vertex(sol_idxs[0], g);
		cost = d[vidx];
		for (i = 1; i < sol_idxs.size(); ++i) {
			if (d[sol_idxs[i]] < cost) {
				ret = sol_idxs[i];
				vidx = boost::vertex(ret, g);
				cost = d[vidx];
			}
		}
		// Trace minimum cost path (0: destination, size()-1: source) 
		std::vector<vertex_descriptor> path;
		vertex_descriptor current = boost::vertex(vidx, g);
		while (current != s) {
			path.push_back(current);
			current = p[current];
		}
		path.push_back(s);
		// Empties mvpath first
		if (mvpath.size() != 0) {
			mvpath.clear();
		}
		// Traverses minimum cost path backward, filling mvpath
		//printf("PATH: START ");
		for (i = path.size() - 1; i > 0; --i) {
			//printf("%i,", path[i]);
			mvpath.push_back(g[boost::edge(path[i], path[i - 1], g).first].mv);
		}
		//printf("%i END\n", path[i]);
		return ret;
	}
};