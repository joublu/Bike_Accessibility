#include "Graph.h"
#include "Tile.h"
#include "Utils.h"
#include "Tiles.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

Edge* Graph::getGivenEdge(long int i, long int j){
	for (std::vector<Edge>::iterator it = list_of_edges.begin(); it != list_of_edges.end(); it++){
		if ((*it).get_node_id_1() == i && (*it).get_node_id_2() == j)
			return &(*it);
	}
	return nullptr;
}

// Suppose que les edge sont rang�s dans l'ordre de leur id, id == position dans le vecteur >> pour gagner du temps et ne pas parcourir le vecteur
Edge* Graph::getGivenEdge(long int id)
{
	return &(list_of_edges[id]);
}

// pas utilisé
void Graph::compute_reachable_edges(Tile* currTile, float dist_limit)
{
	//cout << "enter get reachable " << endl;
	reset_nodes();

	// Stack des noeuds � explorer
	std::vector<Node*> nodes_stack;

	// Noeud de d�part du Dijkstra
	long int current_node_id = currTile->getIdcentralNode();
	Node* current_node_ptr = getPtrNode(current_node_id);
	current_node_ptr->setIsVisisted(true);
	current_node_ptr->setDistance(0);
	double curr_dist = 0;

	// Ajout du noeud de depart dans la stack
	nodes_stack.push_back(current_node_ptr);

	do {

		// Retirer le neoud de queue (le vecteur est tri� par distance d�croissante)
		current_node_ptr = nodes_stack.back();
		curr_dist = current_node_ptr->getDist();
		current_node_id = current_node_ptr->getId();
		nodes_stack.pop_back();
		//cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		//Parcourir les successseurs du noeud courant
		for (int j = 0; j < successors[current_node_id].size(); j++)
		{
			Node* tmp_ptr_node = getPtrNode(successors[current_node_id][j]);

			Edge* curr_edge = this->getGivenEdge(current_node_id, tmp_ptr_node->getId() );
			//cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite et si ce noeud n'�tait pas d�j� atteint avec une plus petite distance
			if ( (curr_dist + curr_edge->get_edge_cost_1() <= dist_limit) &&	(curr_dist + curr_edge->get_edge_cost_1() < tmp_ptr_node->getDist()))
			{
				// Mise � jour du nouveau label distance pour ce voisi
				tmp_ptr_node->setDistance(curr_dist + curr_edge->get_edge_cost_1());
				//Ajout du voisin dans la liste � explorer si pas deja present
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);

					// Ajouter l'arc dans la liste des arcs atteignables par pour la Tile
					currTile->getEdgeVisibility().push_back(curr_edge);
				}
			}
		}

		// Trier le vecteur par ordre d�crosisant des distance
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste
		
		/*for (int i = 0; i < nodes_stack.size(); i++)
			cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		cout << "  ---- " << endl;*/
	} while (nodes_stack.size() > 0);
	
	// TODO: ins�rer une instruction return ici

}

// sert a clc la visibilité, à revoir
void Graph::compute_reachable_edges_v2(Tile* currTile, float dist_limit)
{
	//cout << "enter get reachable " << endl;
	//reset_nodes(); // le constructeur met deja la distance à infinity et

	// Stack des noeuds � explorer
	std::vector<Node*> nodes_stack;
	std::vector<Node*> nodes_stack_to_reset;

	// Noeud de d�part du Dijkstra
	long int current_node_id = currTile->getIdcentralNode();
	Node* current_node_ptr = getPtrNode(current_node_id);
	current_node_ptr->setIsVisisted(true);
	current_node_ptr->setDistance(0);
	double curr_dist = 0;

	// Ajout du noeud de depart dans la stack
	nodes_stack.push_back(current_node_ptr);
	nodes_stack_to_reset.push_back(current_node_ptr);

	do {
		// Retirer le neoud de queue (le vecteur est tri� par distance d�croissante)
		current_node_ptr = nodes_stack.back(); // 1ere iteration c'est le noeud ligne 109
		curr_dist = current_node_ptr->getDist(); // 1ere iteration c'est 0
		current_node_id = current_node_ptr->getId();
		nodes_stack.pop_back(); // destroys the elem
		//cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles (cf l140))
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{

			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			//cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite et si ce noeud n'�tait pas d�j� atteint avec une plus petite distance
			if ((curr_dist + curr_edge->get_edge_cost_1() <= dist_limit) && (curr_dist + curr_edge->get_edge_cost_1() < tmp_ptr_node->getDist()))
			{
				// Mise � jour du nouveau label distance pour ce voisi
				tmp_ptr_node->setDistance(curr_dist + curr_edge->get_edge_cost_1());
				//Ajout du voisin dans la liste � explorer si pas deja present
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);
					nodes_stack_to_reset.push_back(tmp_ptr_node);

					// Ajouter l'arc dans la liste des arcs atteignables par pour la Tile
					currTile->getEdgeVisibility().push_back(curr_edge);
					
				}
			}
		}

		// Trier le vecteur par ordre d�crosisant des distance
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste

		/*for (int i = 0; i < nodes_stack.size(); i++)
			cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		cout << "  ---- " << endl;*/
	} while (nodes_stack.size() > 0);

	// TODO: ins�rer une instruction return ici
	// reset nodes
	reset_list_of_nodes(nodes_stack_to_reset);
}

// nouveau
void Graph::compute_reachable_edges_v3(Tile* currTile, float dist_limit)
{
	//cout << "enter get reachable " << endl;
	//reset_nodes(); // le constructeur met deja la distance à infinity et

	// Stack des noeuds � explorer
	std::vector<Node*> nodes_stack;
	std::vector<Node*> nodes_stack_to_reset;

	// Noeud de d�part du Dijkstra
	long int current_node_id = currTile->getIdcentralNode();
	Node* current_node_ptr = getPtrNode(current_node_id);
	current_node_ptr->setIsVisisted(true);
	current_node_ptr->setDistance(0);
	double curr_dist = 0;

	// Ajout du noeud de depart dans la stack
	nodes_stack.push_back(current_node_ptr);
	nodes_stack_to_reset.push_back(current_node_ptr);

	do {
		// Retirer le neoud de queue (le vecteur est tri� par distance d�croissante)
		current_node_ptr = nodes_stack.back(); // 1ere iteration c'est le noeud ligne 109
		curr_dist = current_node_ptr->getDist(); // 1ere iteration c'est 0
		current_node_id = current_node_ptr->getId();
		nodes_stack.pop_back(); // destroys the elem
		//cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles (cf l140))
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{

			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			//cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite et si ce noeud n'�tait pas d�j� atteint avec une plus petite distance
			if ((curr_dist + curr_edge->get_edge_cost_1() <= dist_limit) && (curr_dist + curr_edge->get_edge_cost_1() < tmp_ptr_node->getDist()))
			{
				// Mise � jour du nouveau label distance pour ce voisi
				tmp_ptr_node->setDistance(curr_dist + curr_edge->get_edge_cost_1());
				//Ajout du voisin dans la liste � explorer si pas deja present
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);
					nodes_stack_to_reset.push_back(tmp_ptr_node);

					// Ajouter l'arc dans la liste des arcs atteignables par pour la Tile
					currTile->getEdgeVisibility().push_back(curr_edge);
					
				}
			}
		}

		// Trier le vecteur par ordre d�crosisant des distance
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste

		/*for (int i = 0; i < nodes_stack.size(); i++)
			cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		cout << "  ---- " << endl;*/
	} while (nodes_stack.size() > 0);

	// TODO: ins�rer une instruction return ici
	// reset nodes
	reset_list_of_nodes(nodes_stack_to_reset);
}

void Graph::initialize_tiles_visibility_set(Tiles* carreaux, float dist_limit)
{
	reset_nodes();
	int nbTiles = carreaux->getNbTiles();
	for (int i = 0; i < nbTiles; i++)
	{
		//cout << "TILE " << i << endl;
		this->compute_reachable_edges_v2(carreaux->getListeOfTiles()[i], dist_limit);
	}
}

void Graph::populate_successors_precessors(){
	//std::cout << "enter populate with list_of_edges.size()=" << list_of_edges.size() << std::endl;
	for (long int e = 0; e < list_of_edges.size(); e++){
		successors[list_of_edges[e].get_node_id_1()].push_back(list_of_edges[e].get_node_id_2());
		predecessors[list_of_edges[e].get_node_id_2()].push_back(list_of_edges[e].get_node_id_1());

		edges_successors[list_of_edges[e].get_node_id_1()].push_back(&(list_of_edges[e]));
		edges_predecessors[list_of_edges[e].get_node_id_2()].push_back(&(list_of_edges[e]));
	}

	// display successors and predecessors
	/*for (auto x : successors)
	{
		std::cout << "successors of " << x.first << " : ";
		for (int i = 0; i < x.second.size(); i++)
			std::cout << x.second[i] << "  ";
		std::cout << std::endl;

	}
	cout << "nb succ of 10692 =" << successors[10692].size() << endl;
	cout << "nb pred of 10692 =" << predecessors[10692].size() << endl;

	for (auto x : predecessors)
	{
		std::cout << "predecessors of " << x.first << " : ";
		for (int i = 0; i < x.second.size(); i++)
			std::cout << x.second[i] << "  ";
		std::cout << std::endl;

	}*/

}

bool Graph::doPathexists(long int start, long int end){
	//std::cout << "enter doPathexists " << std::endl;
	// Init isVisited to flase for each nodes
	for (long int i = 0; i < list_of_nodes.size(); i++)
		list_of_nodes[i].setIsVisisted(false);

	bool founded = false;
	std::vector<int> nodes_to_explore;
	nodes_to_explore.push_back(start);
	list_of_nodes[start].setIsVisisted(true);
	while (founded == false && !(nodes_to_explore.empty()))
	{
		int current_node = nodes_to_explore[0];
		nodes_to_explore.erase(nodes_to_explore.begin());
		//std::cout << "current node = " << current_node << " successors size = " << successors[current_node].size() << std::endl;
		for (int j = 0; j < successors[current_node].size(); j++)
		{
			int succ = successors[current_node][j];
			//std::cout << "current succ = " << succ << std::endl;
			if (succ == end)
			{
				founded = true;
				break;
			}
			else
			{
				if (!(list_of_nodes[succ].getIsVisited()))
				{
					nodes_to_explore.push_back(succ);
					list_of_nodes[succ].setIsVisisted(true);
					//std::cout << succ << "added to liste to explore" << std::endl;
				}
			}
		}
	}

	return founded;
}

// pas utilisé
void Graph::findPossibleODPairs(int nbPairs){
	std::ofstream pairsFile;
	pairsFile.open("./Graph_data/sf_pairs.csv", ios::out);
	if (!pairsFile.is_open())
	{
		cout << "error opening pairs file" << endl;
		return;
	}

	//std::vector<std::pair<int, int>> validODPairs;
	int founded = 0;
	do {
		int start = rand() % this->getNbNodes();
		int end = rand() % this->getNbNodes();
		double alpha = ((double)rand() / (RAND_MAX));
		if (doPathexists(start, end))
		{
			std::cout << start << " - " << end << " is a valid OD pair" << std::endl;
			//validODPairs.push_back(std::pair<int, int>(start, end));

			pairsFile << start << "\t" << end << "\t" << alpha << endl;
			founded++;
		}
		else
		{
			std::cout << start << " - " << end << " is NOT a valid OD pair" << std::endl;
		}
	} while (founded < nbPairs);
	return;
}

long int Graph::getIdForNameNode(long int nameNode){
    long int idSolution = 0;
    while (((list_of_nodes[idSolution]).getName() != nameNode) && idSolution<nb_nodes)
        idSolution ++;
    return idSolution;
}

void Graph::reset_nodes()
{
	for (int i = 0; i < list_of_nodes.size(); i++)
	{
		list_of_nodes[i].setIsVisisted(false);
		list_of_nodes[i].setDistanceToInfinity();
	}
}

void Graph::reset_list_of_nodes(std::vector<Node*> l)
{
	for (int i = 0; i < l.size(); i++)
	{
		l[i]->setIsVisisted(false);
		l[i]->setDistanceToInfinity();
	}
}



