#pragma once
#include "Edge.h"
#include <vector>

// un seul pcc
class PCC 
{
private:
	int id;
	int z; // noeud délégué carreau (noeud de départ)
	int p; // noeud POI (noeud d'arrivée)
    double dist; // distance du PCC
	std::vector<Edge*> edges_of_pcc; // liste des arêtes du PCC

public:
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