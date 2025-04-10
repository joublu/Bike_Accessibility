#pragma once

#include "Graph.h"
#include "Tiles.h"
#include <string>
#include <ilcplex/ilocplex.h>
#include <boost/functional/hash.hpp>
#include <algorithm>



struct key_type {
	Tile* tile;
	POI* poi;
	Edge* edge;
	bool operator== (const key_type& other) const {
		return tile == other.tile && poi == other.poi && edge == other.edge;
	}
};

struct couple_type {
	Tile* tile;
	POI* poi;
	bool operator== (const couple_type& other) const {
		return tile == other.tile && poi == other.poi;
	}
};

// revoir pq utiliser un template
namespace std {
	template<>
	struct hash<key_type> {
		size_t operator() (const key_type& key) const {
			size_t h = 0;
			boost::hash_combine(h, key.tile);
			boost::hash_combine(h, key.poi);
			boost::hash_combine(h, key.edge);
			return h;
		}
	};

	template<>
	struct hash<couple_type> {		
		size_t operator() (const couple_type& key) const {
			size_t h = 0;
			boost::hash_combine(h, key.tile);
			boost::hash_combine(h, key.poi);
			return h;
		}
	};
}

class ModelCplex_BA
{
private:
	std::string nameModel;

	//** Attributs du mod�le **//
	IloEnv env;
	float budget;
	double LTS_max;
	float distance_max;

	// Graph's attributs
	Graph* graph;
	long int nbNodes;
	long int nbEdges;

	// Tiles's attributs
	Tiles* carreaux;
	long int nbZones;	
	long int nbPPOI;
	long int max_pz_pair;
	long int max_ppoi;

	// Mapping for var indexes
	std::unordered_map<key_type, size_t> idx_mapping;
	long int map_size;
	std::unordered_map < couple_type, size_t > couple_idx_mapping;
	long int couple_map_size;

	// Objective's attribut
	IloExpr objectiveExpr;
	IloObjective objectiveVariable;
	

	// Variable's attribut
	IloModel model;
	IloNumVarArray Delta_var;
	IloNumVarArray Y_var;
	IloNumVarArray YB_var;
	IloNumVarArray SB_var;
		
	IloNumVarArray D_var;//D : distance between a poi and a zone
	IloNumVarArray PPOI_var; // PPOI barre : indique si le poi est atteignable par z dans la limite de distance donn�e

	// Cplex attribut
	IloCplex cplex;
	double resolutionTime;
	double modelBuildingTime;
	IloExpr budgetExpr;
	IloConstraint budgetConstraint;

	IloExpr sum_edges_costs;

	IloConstraintArray c13Constraints;

	//double importTimeGraph, importTimeOD;
	//double resolutionTime, totalTimeModel;
	//double bestObjectiveValue, feasibleSolutionTime, optimumSolutionTime, optimabilityProofTime;

	//IloNumVarArray solutionVariables;
	//IloNumArray solutionValues;
	//vector<Edge> modified_edge;

	

public:
	/// Dmax doit avoir la m�me valeur que la fonction qui a intialis�e la visibilit� des carreaux
	ModelCplex_BA(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax);
	~ModelCplex_BA();

	void generate_variables_model_v2();

	void generate_variables_model();
	void generate_constraints();
	void createObjective();
	void createObjectiveOnPopulation();
	void createObjectiveOnDistance();
	void printVisiblePPOI();
	void printVisiblePPOIToFile(const std::string& filename1, const std::string& filename2);
	void solveModelSmallVisibility(bool affichage, bool needExport, bool setOffPreSolve);
	void solveModelExact(bool affichage, bool needExport, bool setOffPreSolve);

	void createBudgetConstraint();
	void changeBudgetConstraint( float _newBudget);

	void changeC13Constraints(double newLTSmax);


	string createFileNameSmallVisibility();
	string createFileNameExact();
};

