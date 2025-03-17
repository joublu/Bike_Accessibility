#pragma once

#include "Graph.h"
#include "Tiles.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h> // to compute wall clock time

using namespace std;

class PCC2
{
private:
	// int id;
	// int z; // noeud délégué carreau (noeud de départ)
	// int p; // noeud POI (noeud d'arrivée)
	// double dist; // distance du PCC
	// std::vector<Edge*> edges_of_pcc; // liste des arêtes du PCC

	// long int couple_map_size;
	// long int map_size;

	float budget;
	double LTS_max;
	float distance_max;

	double resolutionTime;

	int ppoi_barre;

	Graph* graph;
	Tiles* carreaux;

public:
	PCC2(float _b, double _ltsmax, float _dmax, Graph* _g, Tiles* _t) : budget(_b), LTS_max(_ltsmax), distance_max(_dmax), graph(_g), carreaux(_t) {
		ppoi_barre=0;
		resolutionTime=0;
	};

	// int compute_objective();
	void compute_objective();
	void solveModel();
	string createFileName();
};

