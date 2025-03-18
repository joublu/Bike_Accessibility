#pragma once

#include "Node.h"
#include "Edge.h"
#include "Tile.h"
#include "Tiles.h"
#include <vector>
#include <unordered_map>
#include <iostream>

// un seul pcc
struct PCC {
	int id;
	int z; // noeud délégué carreau (noeud de départ)
	int p; // noeud POI (noeud d'arrivée)
    double dist; // distance du PCC
	std::vector<Edge*> edges_of_pcc; // liste des arêtes du PCC

	// constructeur
	PCC(long int _id, int _z, int _p, double _dist) : id(_id), z(_z), p(_p), dist(_dist) {};
	PCC(long int _id, int _z, int _p, double _dist, std::vector<Edge*> _edges_of_pcc) : id(_id), z(_z), p(_p), dist(_dist) {
		edges_of_pcc = _edges_of_pcc;
	};
	friend std::ostream& operator<<(std::ostream& os, const PCC& pcc) {
		os << "PCC(";
		os << "id: " << pcc.id << ", ";
		os << "z: " << pcc.z << ", ";
		os << "p: " << pcc.p << ", ";
		os << "dist: " << pcc.dist << ", ";
		os << "edges: [";
		for (size_t i = 0; i < pcc.edges_of_pcc.size(); ++i) {
			os << pcc.edges_of_pcc[i]->get_node_id_1() << "-" << pcc.edges_of_pcc[i]->get_node_id_2() << "; ";
		}
		os << "])";
		return os;
	}
	// edges_of_pcc(_edges_of_pcc);
	std::vector<Edge*> getPath () { return edges_of_pcc; };
	double getDist() const { return dist; };
};

class Graph
{
private:
	string g_name;
	int nb_nodes;
	int nb_edges;
	std::vector<Node> list_of_nodes;
	std::vector<Edge> list_of_edges;
	// std::vector<std::pair<std::pair<Node*,Node*>,>> PCCs; // pour l'heuristique

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
	bool doSecurePathExists(long int start, long int end, double lts_max, double dist_limit);
	void findPossibleODPairs(int );

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
	void initialize_tiles_visibility_set(Tiles* carreaux, float dist);
	void compute_reachable_edges_h(Tile* currTile, float dist_limit);
	void initialize_tiles_visibility_set_h(Tiles* carreaux, float dist);
	void find_edges_to_change(Tiles* carreaux, float _b, double _ltsmax, float _dmax);
};

