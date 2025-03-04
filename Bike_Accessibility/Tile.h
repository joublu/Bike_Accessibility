#pragma once

#include "Edge.h"
#include "POI.h"
#include "Node.h"

#include <vector>

using namespace std;

class Tile {

private:
	long int id_tile;
	long int id_central_node;
	long int tile_population;
	vector<Node*> tile_node_visibility;
	vector <Edge*> tile_edge_visibility;
	vector <POI*> tile_potential_poi;
public:
	Tile(long int _id, long int _id_node, long int _tile_population): id_tile(_id), id_central_node(_id_node), tile_population(_tile_population) {};

	//getters
	vector<Node*>& getNodeVisibility() { return tile_node_visibility; };
	vector<Edge*>& getEdgeVisibility() { return tile_edge_visibility; };
	vector<POI*>& getPotentialPoi() { return tile_potential_poi; };

	long int getIdTile() { return id_tile; };
	long int getIdcentralNode() { return id_central_node; };
	long int getTilePopulation() { return tile_population; };
	//setters

	//

};
