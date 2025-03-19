#pragma once

#include "Graph.h"
#include "Tiles.h"
#include "PCC.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h> // to compute wall clock time

using namespace std;

class HeuristicPCC
{
private:
	float budget;
	double LTS_max;
	float distance_max;
	Graph* graph;
	Tiles* carreaux;

	double modelBuildingTime;
	double resolutionTime;

	int ppoi_barre;
	vector<PCC*> pccs; // contains all PCCs of the graph (from a central node to a PPOI)

public:
	// HeuristicPCC(float _b, double _ltsmax, float _dmax, Graph* _g, Tiles* _t) : budget(_b), LTS_max(_ltsmax), distance_max(_dmax), graph(_g), carreaux(_t) {
	// 	ppoi_barre=0;
	// 	resolutionTime=0;
	// };
	HeuristicPCC(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax);

	void find_edges_to_change();

	// int compute_objective();
	int compute_objective();
	void solveModel();
	string createFileName();
};

