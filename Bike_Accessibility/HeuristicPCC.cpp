#include "HeuristicPCC.h"
#include "Utils.h"
#include <algorithm>

using namespace std;


HeuristicPCC::HeuristicPCC(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax)
{
    budget = _b;
    LTS_max = _ltsmax;
    distance_max = _dmax;
    graph = _g;
    carreaux = _t;
    ppoi_barre = 0;
    resolutionTime = 0;

    int id=0;
    int nb_tiles = carreaux->getNbTiles();
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
					path.insert(path.begin(), _g->getGivenEdge(pred->getId(), here)); // va jusqu'à z (après plus de pred)
					here = pred->getId();
					pred= curr_tile->getPredAndDistForID(here).first;
				}
				// stocker le PCC
				PCC* new_pcc = new PCC(id, central_node_id, curr_node_id, dist, path); // &PCC(id, curr_node_id, central_node_id, dist, path);
				pccs.push_back(new_pcc);
				
			}
			id++;
		}
	}

    int cpt_nz_is_np=0;
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* tile_ptr = carreaux->getListeOfTiles()[z];
        int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
        for (int p = 0; p < nb_ppoi; p++)
        {
            POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];
            long int node_poi_np = poi_ptr->getPoiNode();
            if (tile_ptr->getIdcentralNode() == node_poi_np)
            {
                cpt_nz_is_np++;
            }
        }
    }
    cout << "cpt_nz_is_np = " << cpt_nz_is_np << endl;
    long int map_size = carreaux->getsizeVarTab();
    cout << "map_size = " << map_size << endl;
}

/**
 * Heuristique
 * Pour tous les couples noeuds-tildes, calculer le plus court chemin entre le couple
 * Trier par reste à aménager
 * 	Tant qu'il reste du budget
 * 		Compléter le 1er
 * 		Recalculer le score
 */
// this one should happen after initialize_reachable_poi_v2
// voir si possible de fusionner ca avec une fonction similaire à
// Tiles::initialize_reachable_poi_v2
// (en terme de complexité c'est pas le plus couteux)
void HeuristicPCC::find_edges_to_change()
{
	// 1ere partie : tri de ppcs
	// tri décroissant
	std::sort(pccs.begin(), pccs.end(), myUtils::sortbydecreasdistPCC);
	// cout<<"affichage des pccs apres tri"<<endl;
	// for(int i = 0; i < pccs.size(); i++)
	// {
	// 	cout << *pccs[i] << endl;
	// }

	// on met à "improved" les edges qui ont deja le bon lts 
	for(int i = 0; i < graph->getListOfEdges().size(); i++)
	{
		Edge curr_edge = graph->getListOfEdges()[i];
		if (curr_edge.get_edge_LTS() <= LTS_max)
		{
			curr_edge.set_is_improved(true);
		}
	}

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
				if (curr_edge->get_is_improved() == false && curr_edge->get_edge_LTS() > LTS_max)
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
	}
}

int HeuristicPCC::compute_objective()
{
    // graph->find_edges_to_change(carreaux, budget, LTS_max, distance_max); // constructeur et heuristique 
    int PPOI_var = 0; // ce que va varier
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        int PPOI_visible = curr_tile->getPotentialPoi().size(); // nombre de ppoi visibles
        for (int p = 0; p < PPOI_visible; p++)
        {
            PPOI_var++; // le ppoi est visible
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            if (graph->doSecurePathExists(z, poi_ptr->getPoiNode(), LTS_max, distance_max))
            {
                PPOI_var--; // le ppoi est atteint
            }
        }
    }
    return PPOI_var;
}

string HeuristicPCC::createFileName()
{
    string filename = "./Results/";
    filename += graph->getGraphName() + "_heuristique_";

    std::string stringBudget = std::to_string(budget);
    stringBudget = stringBudget.substr(0, stringBudget.find('.'));

    std::string stringLTS = std::to_string(LTS_max);
    stringLTS = stringLTS.substr(0, stringLTS.find('.') + 3);
    
    std::string stringDmax = std::to_string(distance_max);
    stringDmax = stringDmax.substr(0, stringLTS.find('.') + 3);


    filename += "Budget_" + stringBudget;
    filename += "_LTSmax_" + stringLTS;
    filename += "_Dmax_" + stringDmax;
    filename += ".csv";
    std::cout << "filename = " << filename << endl;
    return filename;
}

void HeuristicPCC::solveModel() {

    cout << "enter solve" << endl;

    // cout << "=========================================================================" << endl;

    // env.out() << "Solution value = " << cplex.getObjValue() << endl;

    resolutionTime = 0;
    clock_t start, finish;
    start=clock();

    find_edges_to_change(); // constructeur et heuristique 

    int PPOI_var = 0; // ce que va varier

    // mettre ca ds une nouvelle fonction
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        int PPOI_visible = curr_tile->getPotentialPoi().size(); // nombre de ppoi visibles
        // cout << "PPOI_visible pour " << curr_tile->getIdTile() << " = " << PPOI_visible << endl;
        for (int p = 0; p < PPOI_visible; p++)
        {
            PPOI_var++; // le ppoi est visible
            // cout<<"PPOI_var = "<<PPOI_var<<endl;
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            // cout << "z p = " << curr_tile->getIdcentralNode() << " " << poi_ptr->getPoiNode() << endl;
            if (graph->doSecurePathExists(curr_tile->getIdcentralNode(), poi_ptr->getPoiNode(), LTS_max, distance_max)) // voir direction
            {
                // cout << "path exists" << endl;
                PPOI_var--; // le ppoi est atteint
            }
            // cout<<"PPOI_var = "<<PPOI_var<<endl;
        }
    }
    ppoi_barre=PPOI_var; // revoir pr utiliser ca directement

    finish=clock();
    resolutionTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Temps de résolution: " << resolutionTime << endl;

    cout << "Solution value  = " << ppoi_barre << endl;

    std::ofstream resFile;
    resFile.open(createFileName(), ios::out);

    long int cpt_arcs_amenages = 0;
    size_t e = 0;
    for (auto it = graph->getListOfEdges().begin(); it != graph->getListOfEdges().end(); it++)
    {
        
        int ni = it->get_node_id_1();
        int nj = it->get_node_id_2();
        if (it->get_is_improved())
        {
            cout << "SB_e_" << ni << "_" << nj << " is improved" << endl;
            cpt_arcs_amenages++;
        }
        e++;
    }  

    if (!resFile.is_open())
    {
        std::cout << "error opening results file" << endl;
        return;
    }
 
    resFile << "graph_name" << ";" << graph->getGraphName() << endl;
    resFile << "nbTiles" << ";" << carreaux->getNbTiles() << endl;
    resFile << "nbPoi" << ";" << carreaux->getNbPoi() << endl;
    resFile << "nbPoiTileCouple" << ";" << carreaux->getNbPpoiTileCouple() << endl;

    resFile << "budget" << ";" << budget << endl;
    resFile << "LTS_max" << ";" << LTS_max << endl;
    resFile << "distance_max" << ";" << distance_max << endl;

    resFile << "modelBuildingTime" << ";" << 0 << endl;
    resFile << "resolutionTime" << ";" << resolutionTime << endl;
    resFile << "objective_value" << ";" << ppoi_barre << endl;
    resFile << "#arcs amenages" << ";" << cpt_arcs_amenages << endl;

    resFile << "arc_to_improve" << "; " << endl;

    e = 0;
    for (auto it = graph->getListOfEdges().begin(); it != graph->getListOfEdges().end(); it++)
    {
        
        int ni = it->get_node_id_1();
        int nj = it->get_node_id_2();
        if (it->get_is_improved())
        {
            resFile << ni << ";" << nj << endl;
        }
        e++;
    }  
 
}