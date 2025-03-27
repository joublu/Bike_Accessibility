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
	float budget_left;
	double LTS_max;
	float distance_max;
	Graph* graph;
	Tiles* carreaux;

	double modelBuildingTime;
	double resolutionTime;

	int ppoi_barre;
	vector<PCC*> pccs; // contains all PCCs of the graph (from a central node to a PPOI)

public:
	HeuristicPCC(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax);

	void find_edges_to_change();
	void find_edges_to_change_fill_budget();

	// int compute_objective();
	void compute_objective();
	// void compute_objective_with_pccs();
	int compute_objective_with_pccs();
	void solveModel();
	void solveModelFillBudget();
	string createFileName();
	string createFileNameFillBudget();
};

