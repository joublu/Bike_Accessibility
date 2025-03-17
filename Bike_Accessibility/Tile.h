#pragma once

#include "Edge.h"
#include "POI.h"
#include "Node.h"

#include <vector>
#include <unordered_map>

using namespace std;

class Tile {

private:
	long int id_tile;
	long int id_central_node;
	long int tile_population;
	vector<Node*> tile_node_visibility;
	vector <Edge*> tile_edge_visibility;
	vector <POI*> tile_potential_poi;
	// vector<std::pair<Edge*, int>> path_and_dist; // stocker le pred & dist de id_central_node a chaque noeud visible
	unordered_map<long int, std::pair<Node*, double>> pred_and_dist; //a chaque noeud visible, stocker le pred & dist de id_central_node
public:
	Tile(long int _id, long int _id_node, long int _tile_population): id_tile(_id), id_central_node(_id_node), tile_population(_tile_population) {};

	//getters
	vector<Node*>& getNodeVisibility() { return tile_node_visibility; }; // voir ce que signifie la ref & exactement
	vector<Edge*>& getEdgeVisibility() { return tile_edge_visibility; };
	vector<POI*>& getPotentialPoi() { return tile_potential_poi; };
	unordered_map<long int, std::pair<Node*, double>>& getPredAndDist() { return pred_and_dist; };
	std::pair<Node*, double>& getPredAndDistForID(long int _id) { return pred_and_dist[_id]; };

	long int getIdTile() { return id_tile; };
	long int getIdcentralNode() { return id_central_node; };
	long int getTilePopulation() { return tile_population; };
	//setters

	//

};
