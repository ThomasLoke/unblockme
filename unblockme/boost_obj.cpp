#include "stdafx.h"

#include "boost_obj.h"
using namespace std;

boost_obj::boost_obj(void) {}

boost_obj::boost_obj(const boost_obj& inst) {
	g = inst.g;
	source = inst.source;
	mvpath = inst.mvpath;
}

boost_obj::~boost_obj(void) {}

void boost_obj::clear(void) {
	g.clear();
	source = 0;
	mvpath.clear();
}

void boost_obj::print(void) {
	printf("Graph properties:\nNumber of vertices - %i\nNumber of directed edges - %i\n", boost::num_vertices(g), boost::num_edges(g));
	if (mvpath.size() > 0) {
		printf("Current solution path from source to destination uses %i moves\n", mvpath.size());
	}
}

bool boost_obj::g_add_edge(int idx_l, int idx_r, rmove mv) {
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

bool boost_obj::g_add_edge_single(int idx_l, int idx_r, rmove mv) {
	// Adds the directed edge l-r with weight 1 and with the associated move
	return boost::add_edge(idx_l, idx_r, { 1,mv }, g).second;
}

int boost_obj::find_shortest_path(const vector<int>& sol_idxs) {
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