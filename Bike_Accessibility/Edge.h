#pragma once

#include <limits>
#include <iostream>

class Edge
{
private:
	long int edge_id;
	long int node_id_1;
	long int node_id_2;
	double edge_cost_1; // distance (c_ij^1)
	double edge_cost_2; // danger (sert pr clc LTS)
	double LTS;
	double edge_cost_after_1;
	double edge_cost_after_2;
	bool is_improved; // pour l'heuristique

public:
	Edge(long int _id, long int _id_1, long int _id_2, double _cost_1, double _cost_2, double _cost_a_1=0, double _cost_a_2=0) : edge_id(_id), node_id_1(_id_1), node_id_2(_id_2), edge_cost_1(_cost_1), edge_cost_2(_cost_2) , edge_cost_after_1(_cost_a_1), edge_cost_after_2(_cost_a_2){
		
		//Calcule du LTS > v�rification si un pb dans le fichier avce un arc � 0 de distance
		if (edge_cost_1 != 0) LTS = edge_cost_2 / edge_cost_1;
		else LTS = std::numeric_limits<double>::infinity();
		//std::cout << "edge lts = " << LTS << std::endl;
		
		if (edge_cost_1 < 0) edge_cost_1 = 0;
		if (edge_cost_2 < 0) edge_cost_2 = 0;

		if (edge_cost_after_1 < 0) edge_cost_after_1 = 0;
		if (edge_cost_after_2 < 0) edge_cost_after_2 = 0;

		is_improved = false;
		
	};

	//getters
	long int get_node_id_1() { return node_id_1; };
	long int get_node_id_2() { return node_id_2; };
	double get_edge_cost_1() { return edge_cost_1; };
	double get_edge_cost_2() { return edge_cost_2; };
	double get_edge_cost_after_1() { return edge_cost_after_1; };
	double get_edge_cost_after_2() { return edge_cost_after_2; };
	double get_edge_LTS() { return LTS; };
	double get_is_improved() { return is_improved; };

	//setters
	void set_edge_cost_1(double value) {edge_cost_1 = value; };
	void set_edge_cost_2(double value) {edge_cost_2 = value; };
	void set_edge_cost_after_1(double value) {edge_cost_after_1 = value; };
	void set_edge_cost_after_2(double value) {edge_cost_after_2 = value; };
	void set_is_improved(bool value) {is_improved=value; };

	inline bool operator== (Edge edge2){
        return ((this->node_id_1 == edge2.get_node_id_1()) && (this->node_id_2 == edge2.get_node_id_2()));
	};
};

