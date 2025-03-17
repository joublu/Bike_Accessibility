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
		// cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles (cf l140))
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{

			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			// cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite et si ce noeud n'était pas déjà atteint avec une plus petite distance
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

		//  for (int i = 0; i < nodes_stack.size(); i++)
		//  	cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		//  cout << "  ---- " << endl;
	} while (nodes_stack.size() > 0);

	// TODO: ins�rer une instruction return ici
	// reset nodes
	// cout << "currTile " << currTile->getIdTile() << endl;
	// cout << "getnodevisibility " << currTile->getNodeVisibility().size() << endl << endl;
	//  cout << "currTile " << currTile->getIdTile() << endl;
	//  for (int i = 0; i < currTile->getEdgeVisibility().size(); i++)
	//  	cout << "node1 = " << currTile->getEdgeVisibility()[i]->get_node_id_1() << " node2 = " << currTile->getEdgeVisibility()[i]->get_node_id_2() << endl;
	//  cout << "  ---- " << endl;
	//  cout << "currTile " << currTile->getIdTile() << " size getEdgeVisibility " << currTile->getEdgeVisibility().size() << " size getNodeVisibility " << currTile->getNodeVisibility().size() << endl << endl;
	reset_list_of_nodes(nodes_stack_to_reset);
}

/**
 * Calcul de la visibilité des aretes pour un carreau donné
 * La fonction calcule les arêtes visibles
 * Différences avec v2 :
 * 	- on ajoute l'arête même si le voisin a déjà été exploré
 * 	- pas de condition "si ce noeud n'était pas déjà atteint avec une plus petite distance"
 */
void Graph::compute_reachable_edges_v4(Tile* currTile, float dist_limit)
{
	// cout << "enter get reachable " << endl;
	// initialement l'attribut dist_label de chaque noeud est à infinity
	// soit grâce au constructeur, soit grâce à reset_nodes

	// Stack des noeuds à explorer
	std::vector<Node*> nodes_stack;
	std::vector<Node*> nodes_stack_to_reset;

	// Noeud de départ du Dijkstra
	long int current_node_id = currTile->getIdcentralNode();
	Node* current_node_ptr = getPtrNode(current_node_id);
	current_node_ptr->setIsVisisted(true);
	current_node_ptr->setDistance(0);
	double curr_dist = 0;

	// Ajout du noeud de depart dans la stack
	nodes_stack.push_back(current_node_ptr);
	nodes_stack_to_reset.push_back(current_node_ptr);

	// récupérer tous les reachables nodes
	do {
		// Retirer le neoud de queue (le vecteur est trié par distance décroissante)
		current_node_ptr = nodes_stack.back();
		curr_dist = current_node_ptr->getDist();
		current_node_id = current_node_ptr->getId();
		nodes_stack.pop_back();
		//  cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles)
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{
			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			//  cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite 
			if ((curr_dist + curr_edge->get_edge_cost_1() <= dist_limit))// && (curr_dist + curr_edge->get_edge_cost_1() < tmp_ptr_node->getDist()))
			{
				// Mise à jour du nouveau label distance pour ce voisin
				tmp_ptr_node->setDistance(curr_dist + curr_edge->get_edge_cost_1());

				// Ajouter l'arc dans la liste des arcs atteignables pour la Tile
				currTile->getEdgeVisibility().push_back(curr_edge);

				// Ajout du voisin dans la liste à explorer si pas deja present
				// a revoir, cf p19
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);
					nodes_stack_to_reset.push_back(tmp_ptr_node);
				}
			}
		}

		// Trier le vecteur par ordre décroissant des distances
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste

		//  for (int i = 0; i < nodes_stack.size(); i++)
		//  	cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		//  cout << "  ---- " << endl;
	} while (nodes_stack.size() > 0);

	// cout << "currTile " << currTile->getIdTile() << endl;
	// cout << "getnodevisibility " << currTile->getNodeVisibility().size() << endl << endl;

	//  for (int i = 0; i < currTile->getEdgeVisibility().size(); i++)
	//  	cout << "node1 = " << currTile->getEdgeVisibility()[i]->get_node_id_1() << " node2 = " << currTile->getEdgeVisibility()[i]->get_node_id_2() << endl;
	//  cout << "  ---- " << endl;
	//  cout << "currTile " << currTile->getIdTile() << " size getEdgeVisibility " << currTile->getEdgeVisibility().size() << " size getNodeVisibility " << currTile->getNodeVisibility().size() << endl << endl;
	reset_list_of_nodes(nodes_stack_to_reset);
}

/**
 * Calcul de la visibilité des aretes pour un carreau donné
 * La fonction calcule les noeuds visibles, puis retourne les arêtes dont les 2 noeuds sont visibles
 */
void Graph::compute_reachable_edges_v3(Tile* currTile, float dist_limit)
{
	// cout << "enter get reachable " << endl;
	// initialement l'attribut dist_label de chaque noeud est à infinity
	// soit grâce au constructeur, soit grâce à reset_nodes

	// Stack des noeuds à explorer
	std::vector<Node*> nodes_stack;
	std::vector<Node*> nodes_stack_to_reset;

	// Noeud de départ du Dijkstra
	long int current_node_id = currTile->getIdcentralNode();
	Node* current_node_ptr = getPtrNode(current_node_id);
	current_node_ptr->setIsVisisted(true);
	current_node_ptr->setDistance(0);
	double curr_dist = 0;

	// Ajout du noeud de depart dans la stack
	nodes_stack.push_back(current_node_ptr);
	nodes_stack_to_reset.push_back(current_node_ptr);

	// récupérer tous les reachables nodes
	do {
		// Retirer le neoud de queue (le vecteur est trié par distance décroissante)
		current_node_ptr = nodes_stack.back();
		curr_dist = current_node_ptr->getDist();
		current_node_id = current_node_ptr->getId();
		nodes_stack.pop_back();
		// cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles (cf l140))
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{
			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			// cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite 
			if ((curr_dist + curr_edge->get_edge_cost_1() <= dist_limit))
			{
				// Mise à jour du nouveau label distance pour ce voisin
				tmp_ptr_node->setDistance(curr_dist + curr_edge->get_edge_cost_1());
				//Ajout du voisin dans la liste à explorer si pas deja present
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);
					nodes_stack_to_reset.push_back(tmp_ptr_node);
				}
			}
		}

		// Trier le vecteur par ordre décroissant des distances
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste

		// for (int i = 0; i < nodes_stack.size(); i++)
		// 	cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		// cout << "  ---- " << endl;
	} while (nodes_stack.size() > 0);

	// récupérer les edges entre les noeuds visibles
	// en parcourant les noeuds visibles (à changer si dmax=5000)
	for (int i = 0; i < currTile->getNodeVisibility().size(); i++)
	{
		Node* node1 = currTile->getNodeVisibility()[i];
		for (int j = 0; j < currTile->getNodeVisibility().size(); j++)
		{
			Node* node2 = currTile->getNodeVisibility()[j];
			Edge* curr_edge = this->getGivenEdge(node1->getId(), node2->getId());
			if (curr_edge != nullptr)
				currTile->getEdgeVisibility().push_back(curr_edge);
		}
	}

	// cout << "currTile " << currTile->getIdTile() << endl;
	// for (int i = 0; i < currTile->getEdgeVisibility().size(); i++)
	// 	cout << "node1 = " << currTile->getEdgeVisibility()[i]->get_node_id_1() << " node2 = " << currTile->getEdgeVisibility()[i]->get_node_id_2() << endl;
	// cout << "  ---- " << endl;
	// cout << "currTile " << currTile->getIdTile() << " size getEdgeVisibility " << currTile->getEdgeVisibility().size() << " size getNodeVisibility " << currTile->getNodeVisibility().size() << endl << endl;
	reset_list_of_nodes(nodes_stack_to_reset);
}

void Graph::initialize_tiles_visibility_set(Tiles* carreaux, float dist_limit)
{
	reset_nodes();
	int nbTiles = carreaux->getNbTiles();
	for (int i = 0; i < nbTiles; i++)
	{
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

// refaire mais en ajoutant une condition sur les edges qu'on ajoute, 
// et ajouter des attributs successors_secure etc à la classe
void Graph::populate_successors_precessors_secure(){
	//std::cout << "enter populate with list_of_edges.size()=" << list_of_edges.size() << std::endl;
	// for (long int e = 0; e < list_of_edges.size(); e++){
	// 	successors[list_of_edges[e].get_node_id_1()].push_back(list_of_edges[e].get_node_id_2());
	// 	predecessors[list_of_edges[e].get_node_id_2()].push_back(list_of_edges[e].get_node_id_1());

	// 	edges_successors[list_of_edges[e].get_node_id_1()].push_back(&(list_of_edges[e]));
	// 	edges_predecessors[list_of_edges[e].get_node_id_2()].push_back(&(list_of_edges[e]));
	// }
}

// a ameliorer pr vitesse
bool Graph::doSecurePathExists(long int start, long int end, double lts_max){
	// cout << start << " " << end << endl;
	if (start==end)
		return true;
	for (long int i = 0; i < list_of_nodes.size(); i++) // voir si utile
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
			Edge* curr_edge = this->getGivenEdge(current_node, succ);
			// if we can use this edge
			// cout << (curr_edge->get_edge_cost_1() <= lts_max) << (curr_edge->get_is_improved()) << endl;
			if (curr_edge->get_edge_LTS() <= lts_max || curr_edge->get_is_improved())
			{				
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
					}
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

/**
 * Heuristique
 * Pour tous les couples noeuds-tildes, calculer le plus court chemin entre le couple
 * Trier par reste à aménager
 * 	Tant qu'il reste du budget
 * 		Compléter le 1er
 * 		Recalculer le score
 */
//this one should happen after initialize_reachable_poi_v2
// voir si possible de fusionner ca avec une fonction similaire à
// Tiles::initialize_reachable_poi_v2
void Graph::find_edges_to_change(Tiles* carreaux, float _b, double _ltsmax, float _dmax)
{
	int somme_without_poi = 0;
	int nb_tiles = carreaux->getNbTiles();
	// int cpt_visible_nodes = 0;
	// int cpt_visible_edges = 0;
	vector<PCC*> pccs;
	int id=0;

	// premiere partie : construire pccs
	for (int t = 0; t < nb_tiles; t++)
	{
		Tile* curr_tile=carreaux->getListeOfTiles()[t];
		int central_node_id = curr_tile->getIdcentralNode();
		// cpt_visible_nodes++;

		//Parcourir les POI attachés à chaque noeud de la visibilité pour les ajouter dans les poi de la tile
		// l'iterateur est un POI*
		for (auto it_poi = curr_tile->getPotentialPoi().begin(); it_poi != curr_tile->getPotentialPoi().end(); it_poi++)
		{
			int curr_node_id = (*it_poi)->getPoiNode();

			// stocker le PCC pcq ce noeud est un POI
			// le noeud actuel ne doit pas etre le noeud central du carreau, pcq il n'a pas de pred
			if (curr_node_id != curr_tile->getIdcentralNode())
			{
				int here = curr_node_id;
				Node* pred= curr_tile->getPredAndDistForID(curr_node_id).first;
				double dist= curr_tile->getPredAndDistForID(curr_node_id).second;
				vector<Edge*> path;
				// trouver le chemin (liste d'edge*)
				while (pred != nullptr)
				{
					path.insert(path.begin(), this->getGivenEdge(pred->getId(), here)); // va jusqu'à z (après plus de pred)
					here = pred->getId();
					pred= curr_tile->getPredAndDistForID(here).first;
				}
				// stocker le PCC
				PCC* new_pcc = new PCC(id, central_node_id, curr_node_id, dist, path); // &PCC(id, curr_node_id, central_node_id, dist, path);
				pccs.push_back(new_pcc);
				
			}
			id++; // voir si utile
		}
	}

	// 2e partie : tri de ppcs
	// tri décroissant
	std::sort(pccs.begin(), pccs.end(), myUtils::sortbydecreasdistPCC);
	cout<<"affichage des pccs apres tri"<<endl;
	for(int i = 0; i < pccs.size(); i++)
	{
		cout << *pccs[i] << endl;
	}

	// on met à aménagé les edges qui ont deja le bon lts 
	for(int i = 0; i < list_of_edges.size(); i++)
	{
		Edge curr_edge = list_of_edges[i];
		if (curr_edge.get_edge_LTS() <= _ltsmax)
		{
			curr_edge.set_is_improved(true);
		}
	}

	float budget=_b;
	// tant qu'il y a du budget on aménage le premier pcc
	// et on calcule le nv budget en prenant bien en compte si des aretes ont déjà été 
	// prises en compte ds le budget
	while (budget>0 && pccs.size()>0)
	{
		PCC* pcc = pccs.back(); // only returns a reference
		vector<Edge*> added_edges; // empty revoir si supp
		bool added_complete_pcc = true;

		for (int i = 0; i < pcc->getPath().size(); i++)
		{
			if (budget>0)
			{
				Edge* curr_edge = pcc->getPath()[i];
				// on ne recompte pas (pr budget) les aretes déjà été aménagées/aretes qui ont le bon lts
				if (curr_edge->get_is_improved() == false && curr_edge->get_edge_LTS() > _ltsmax)
				{
					budget -= curr_edge->get_edge_cost_1();
					curr_edge->set_is_improved(true);
					added_edges.push_back(curr_edge);
				}
			}
			else
			{
				added_complete_pcc = false;
				break;
			}
		}
		// si l'on a parcouru ce pcc en entier, on le pop de la liste des pccs pour continuer de la parcourir 
		if (added_complete_pcc)
		{
			pccs.pop_back();
		}		

		// if (budget < 0)
		// {
		// 	// on remet le pcc dans la liste
		// 	pccs.push_back(pcc);
		// 	break; // berk changer
		// }
		// on ne recompte pas (pr budget) les aretes déjà été aménagées/aretes qui ont le bon lts
		// for (int i = 0; i < pcc->getPath().size(); i++)
		// {
		// 	Edge* curr_edge = pcc->getPath()[i];
		// 	if (curr_edge->get_is_improved() == false && curr_edge->get_edge_LTS() > _ltsmax)
		// 	{
		// 		budget -= curr_edge->get_edge_cost_1();
		// 		curr_edge->set_is_improved(true);
		// 	}
		// }
	}
}

// après avoir clc le pcc entre le central node z et n2, si n2==p, on ajoute l'elem à PPCs 
void Graph::compute_reachable_edges_h(Tile* currTile, float dist_limit)
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
		// cout << "curr  node = " << current_node_id << " with dist label = " << curr_dist  <<  endl;

		// on ajoute le noeud à ceux visibles (pcq ds nodes_stack, d'ou vient current_node_ptr,
		// on ajd seulement des noeuds visibles (cf l140))
		currTile->getNodeVisibility().push_back(current_node_ptr); 

		//Parcourir les successseurs du noeud courant via la liste des edges
		int nb_succ_edges = edges_successors[current_node_id].size();
		for (int j = 0; j < nb_succ_edges; j++)
		{

			Edge* curr_edge = edges_successors[current_node_id][j]; //this->getGivenEdge(current_node_id, tmp_ptr_node->getId());
			Node* tmp_ptr_node = getPtrNode(curr_edge->get_node_id_2());
			// cout << "looking at " << tmp_ptr_node->getId() << " succ of " << current_node_id << " dist i_j = " << curr_edge->get_edge_cost_1() << endl;

			// Si la distance pour atteindre le noeud est dans la limite et si ce noeud n'était pas déjà atteint avec une plus petite distance
			double new_dist = curr_dist + curr_edge->get_edge_cost_1();
			if ((new_dist <= dist_limit) && (new_dist < tmp_ptr_node->getDist()))
			{
				// Mise � jour du nouveau label distance pour ce voisi
				tmp_ptr_node->setDistance(new_dist);
				//Ajout du voisin dans la liste � explorer si pas deja present
				if (tmp_ptr_node->getIsVisited() == false)
				{
					tmp_ptr_node->setIsVisisted(true);
					nodes_stack.push_back(tmp_ptr_node);
					nodes_stack_to_reset.push_back(tmp_ptr_node);

					// Ajouter l'arc dans la liste des arcs atteignables par pour la Tile
					// revoir si déplacer hors du if
					currTile->getEdgeVisibility().push_back(curr_edge);
				}

				// stockage du predecesseur et de la distance au noeud délégué 
				// revoir si l'on modifie bien la map
				currTile->getPredAndDistForID(tmp_ptr_node->getId()) = std::pair<Node*, double>(current_node_ptr, new_dist);
					
			}
		}

		// Trier le vecteur par ordre d�crosisant des distance
		std::sort(nodes_stack.begin(), nodes_stack.end(), myUtils::sortbydecreasdist);

		// Affichage de la liste

		//  for (int i = 0; i < nodes_stack.size(); i++)
		//  	cout << "id = " << nodes_stack[i]->getId() << " dist = " << nodes_stack[i]->getDist() << endl;
		//  cout << "  ---- " << endl;
	} while (nodes_stack.size() > 0);

	// TODO: ins�rer une instruction return ici
	// reset nodes
	// cout << "currTile " << currTile->getIdTile() << endl;
	// cout << "getnodevisibility " << currTile->getNodeVisibility().size() << endl << endl;
	//  cout << "currTile " << currTile->getIdTile() << endl;
	//  for (int i = 0; i < currTile->getEdgeVisibility().size(); i++)
	//  	cout << "node1 = " << currTile->getEdgeVisibility()[i]->get_node_id_1() << " node2 = " << currTile->getEdgeVisibility()[i]->get_node_id_2() << endl;
	//  cout << "  ---- " << endl;
	//  cout << "currTile " << currTile->getIdTile() << " size getEdgeVisibility " << currTile->getEdgeVisibility().size() << " size getNodeVisibility " << currTile->getNodeVisibility().size() << endl << endl;
	reset_list_of_nodes(nodes_stack_to_reset);
}


void Graph::initialize_tiles_visibility_set_h(Tiles* carreaux, float dist_limit)
{
	reset_nodes();
	int nbTiles = carreaux->getNbTiles();
	for (int i = 0; i < nbTiles; i++)
	{
		this->compute_reachable_edges_h(carreaux->getListeOfTiles()[i], dist_limit);
	}
}