#include "Tiles.h"
#include "Node.h"

#include <iostream>


// la fonction initialize_tiles_visibility_set doit d�j� avoir �t� appel�e
// pas utilisée
void Tiles::initialize_reachable_poi()
{
	for (int i = 0; i < nb_poi; i++)
	{
		POI* curr_poi_ptr = list_of_poi[i];
		long int curr_poi_node = list_of_poi[i]->getPoiNode();

		cout << " curr poi id = " << i <<  " poi_node = " << curr_poi_node << endl;
		for (int t = 0; t < nb_tiles; t++)
		{
			//cout << "Tile " << t <<  "edge visibility size " << list_of_tiles[t]->getVisibility().size() << endl;
			for (vector<Edge*>::iterator it = list_of_tiles[t]->getEdgeVisibility().begin(); it != list_of_tiles[t]->getEdgeVisibility().end(); it++)
			{
				if ((*it)->get_node_id_1() == curr_poi_node || (*it)->get_node_id_2() == curr_poi_node)
				{
					// verifier que le poi n'est pas d�j� dans la liste des poi atteignable
					bool inserer = true;
					for(vector<POI*>::iterator it_poi = list_of_tiles[t]->getPotentialPoi().begin(); it_poi != list_of_tiles[t]->getPotentialPoi().end(); it_poi++)
					{
						if ((*it_poi)->getPoiNode() == curr_poi_node)
							inserer = false;
					}

					if (inserer)
						list_of_tiles[t]->getPotentialPoi().push_back(curr_poi_ptr);
				}
			}
			
		}
	}
}

void Tiles::initialize_reachable_poi_v2()
{
	cout << "enter initialize_reachable_poi_v2" << endl;
	/*
	somme_without_poi = nb de carreaux sans POI atteignable
	nb_ppoi_tile_couple = nb initial de couple POI - carreaux 
	nb_attached_poi = diff pr chaque carreau, nb de POI potentiellement attachés à ce carreau
	couple_size_var_tab = somme sur carreaux de nb_attached_poi
	size_var_tab = "Tiles size var tab value"
	*/
	int somme_without_poi = 0;
	nb_ppoi_tile_couple = 0;
	size_var_tab = 0;
	couple_size_var_tab = 0;
	int cpt_visible_nodes = 0;
	int cpt_visible_edges = 0;
	// Pour chaque carreau
	for (int t = 0; t < nb_tiles; t++)
	{
		int nb_attached_poi = 0;
		// Parcourir la liste des noeuds dans la visibilit�
 		for (vector<Node*>::iterator it = list_of_tiles[t]->getNodeVisibility().begin(); it != list_of_tiles[t]->getNodeVisibility().end(); it++)
		{
			cpt_visible_nodes++;
			//Parcourir les POI attach�s � chaque noeud de la visibilit� pour les ajouter dans les poi de la tile
			for (vector<POI*>::iterator it_poi = (*it)->getListOfAttachedPoi().begin(); it_poi != (*it)->getListOfAttachedPoi().end(); it_poi++)
			{
				list_of_tiles[t]->getPotentialPoi().push_back(*it_poi);
				nb_attached_poi++;
				nb_ppoi_tile_couple++;
			}
		}
		cpt_visible_edges += list_of_tiles[t]->getEdgeVisibility().size();
		couple_size_var_tab += nb_attached_poi;
		size_var_tab += nb_attached_poi * list_of_tiles[t]->getEdgeVisibility().size();
		if (nb_attached_poi == 0)somme_without_poi++;
	}
	cout << " TILES WITHOUT REACHABLE POI = " << somme_without_poi << endl;
	cout << " NUMBER OF PPOI AND ZONES COUPLE = " << nb_ppoi_tile_couple << endl; // initial 
	cout << " NUMBER OF size_var_tab" << size_var_tab << endl;
	cout << " cpt_visible_nodes " << cpt_visible_nodes << endl;
	cout << " cpt_visible_edges " << cpt_visible_edges << endl;
}

void Tiles::display_carreaux_data()
{
	int nb_needed_var = 0; // meme truc que size_var_tab ds fonction prec (?)
	for (int t = 0; t < nb_tiles; t++)
	{
		nb_needed_var += list_of_tiles[t]->getEdgeVisibility().size() * list_of_tiles[t]->getPotentialPoi().size();
		std::cout << "TILE " << t << std::endl;
		std::cout << "TILE " << t;
		std::cout << " nbNodes = " << list_of_tiles[t]->getNodeVisibility().size();
		std::cout << " nbEdges_in_visibility " << list_of_tiles[t]->getEdgeVisibility().size();
		std::cout << " nbPPOI_in_visibility " << list_of_tiles[t]->getPotentialPoi().size();
		std::cout << " nb population " << list_of_tiles[t]->getTilePopulation() ;
		std::cout << std::endl;
		/*for (vector<Edge*>::iterator it = list_of_tiles[t]->getEdgeVisibility().begin(); it != list_of_tiles[t]->getEdgeVisibility().end(); it++)
		{
			std::cout << "[ " << (*it)->get_node_id_1() << " - " << (*it)->get_node_id_2() << "]  " << endl;
		}
		std::cout << "TILE Nodes" << std::endl;
		for (vector<Node*>::iterator it = list_of_tiles[t]->getNodeVisibility().begin(); it != list_of_tiles[t]->getNodeVisibility().end(); it++)
		{
			std::cout << (*it)->getId() << " # " ;
		}
		std::cout << std::endl;
		
		std::cout << "TILE PPOI" << std::endl;
		for (vector<POI*>::iterator it = list_of_tiles[t]->getPotentialPoi().begin(); it != list_of_tiles[t]->getPotentialPoi().end(); it++)
		{
			std::cout <<  (*it)->getPoiNode() << "  " << endl;
		}
		std::cout << " ------------------- " << std::endl;
		*/
	}
	//cout << " nb_needed_var = " << nb_needed_var << endl;
	

}
