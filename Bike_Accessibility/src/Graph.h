#pragma once

#include "Node.h"
#include "Edge.h"
#include "Tile.h"
#include "Tiles.h"
#include <vector>
#include <unordered_map>
#include <iostream>

class Graph
{
private:
	string g_name;
	int nb_nodes;
	int nb_edges;
	std::vector<Node> list_of_nodes;
	std::vector<Edge> list_of_edges;

	std::unordered_map<long int, std::vector<long int> > successors;
	std::unordered_map<long int, std::vector<long int> > predecessors;

	std::unordered_map<long int, std::vector<Edge*> > edges_successors;
	std::unordered_map<long int, std::vector<Edge*> > edges_predecessors;

public:
	//getters
	std::vector<Node>& getListOfNodes() { return list_of_nodes; };
	Node& getNode(long int nodeId) { return list_of_nodes[nodeId]; };
	Node* getPtrNode(long int nodeId) { return &(list_of_nodes[nodeId]); };

	std::vector<Edge>& getListOfEdges() { return list_of_edges; };

	int getNbNodes() { return nb_nodes; };
	int getNbEdges() { return nb_edges; };
	std::unordered_map<long int, std::vector<long int> >& getSuccessors(){return successors; };
	std::unordered_map<long int, std::vector<long int> >& getPredecessors() { return predecessors; };

	std::vector<long int>& getSucc(long int _id) { return successors[_id]; };
	std::vector<long int>& getPred(long int _id) { return predecessors[_id]; };
	std::vector<Edge*>& getEdgeSucc(long int _id) { return edges_successors[_id]; };
	std::vector<Edge*>& getEdgePred(long int _id) { return edges_predecessors[_id]; };

	Edge* getGivenEdge(long int i, long int j);
	Edge* getGivenEdge(long int id);

	string getGraphName() { return g_name ; };

	//setters
	void setNbNodes(int _nb) { nb_nodes = _nb; };
	void setNbEdges(int _nb) { nb_edges = _nb; };

	void setGraphName(string name) { g_name = name; };

	//build graph
	void populate_successors_precessors();
	void populate_successors_precessors_secure();

	//find existing paths
	bool doPathexists(long int, long int);
	bool doSecurePathExistsFullVisibility(long int start, long int end, double lts_max, double dist_limit);
	void printVisiblePPOI(Tiles* carreaux, double LTS_max, float distance_max);
	void findPossibleODPairs(int );

	// compute ppoi
	int compute_objective(Tiles* carreaux, double lts_max, float dist_limit);

	// find id when we know the name
	long int getIdForNameNode(long int nameNode);

	// Dikjstra utils
	void reset_nodes();
	void reset_list_of_nodes(std::vector<Node*> l);

	//initialize reachable edge
	void compute_reachable_edges(Tile* currTile, float dist);
	void compute_reachable_edges_v0(Tile* currTile, float dist_limit);
	void compute_reachable_edges_v2(Tile* currTile, float dist_limit);
	void compute_reachable_edges_v3(Tile* currTile, float dist_limit);
	void compute_reachable_edges_v4(Tile* currTile, float dist_limit);
	void compute_reachable_edges_full_visibility(Tile* currTile);
	void initialize_tiles_visibility_set_small_visibility(Tiles* carreaux, float dist);
	void initialize_tiles_visibility_set_exact(Tiles* carreaux, float dist);
	void initialize_tiles_visibility_set_full(Tiles* carreaux, float dist_limit);
	void compute_reachable_edges_h(Tile* currTile, float dist_limit, float lts_max);
	void initialize_tiles_visibility_set_h(Tiles* carreaux, float dist, float lts_max);
};

