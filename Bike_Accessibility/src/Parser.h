#pragma once
#include "Graph.h"
#include "Tiles.h"
//#include "ODPairs.h"
#include <string>

class Parser
{
public:
	// LTS_TYPE = 1 pour les instances de Felix (quand se trouve la colonne cyclability dans le fichier arcs.csv)
	// LTS_TYPE = 0 pour les instances de Jérôme et Alaâ 
	// Interet du param : le rapport danger/distance n'est pas équivalent entre ces 2 types
	Graph* parse_nodes_and_edges_file(std::string path_node, std::string path_edges, char sep, string name, int nbVariante=2,int LTS_TYPE=0);
	double convertLTS(double distance, double danger);

	Tiles* parse_filsofi_file(std::string path_filosofi, char sep);

	void parse_POI_file(std::string path_poi, char sep, Tiles* carreaux, Graph* graph);

	//ODPairs* parse_pairs_file(Graph* g, std::string pathPair, char sep, int limit);

	//vector<long int> parse_trace_order(std::string pathTraceOrder, char sep, int limit);

	//CampagneParameters* parse_campagne_parameter(std::string pathCampagneParameter, char sep);
};

