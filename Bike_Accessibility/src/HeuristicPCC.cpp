#include "HeuristicPCC.h"
#include "Utils.h"
#include <algorithm>

using namespace std;


HeuristicPCC::HeuristicPCC(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax)
{
    budget = _b;
    budget_left = _b;
    LTS_max = _ltsmax;
    distance_max = _dmax;
    graph = _g;
    carreaux = _t;
    ppoi_barre = 0;
    resolutionTime = 0;

    clock_t start, finish;
    start=clock();

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
                // int cmpt=0;
				while (pred != nullptr)
				{
                    // cout << pred->getId() << " " << here << endl;
                    // cmpt++;
					path.insert(path.begin(), _g->getGivenEdge(pred->getId(), here)); // va jusqu'à z (après plus de pred)
					here = pred->getId();
					pred= curr_tile->getPredAndDistForID(here).first;
				}
                // cout << "cmpt = " << cmpt << endl;
				// stocker le PCC
                // revoir ca pcq prends tant de temps (?)
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

    finish=clock();
    modelBuildingTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "modelBuildingTime: " << modelBuildingTime << endl;
}

/**
 * Heuristique
 * Pour tous les couples noeuds-tildes, calculer le plus court chemin entre le couple
 * Trier par distance z -> p ou par reste à aménager en fonction de compute_reachable_edges_h
 * 	Tant qu'il reste du budget
 * 		Compléter le 1er
 * 		Recalculer le score
 */
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

	// tant qu'il y a du budget on aménage le premier pcc et on calcule le nv budget
    // en prenant bien en compte si des aretes ont déjà été prises en compte ds le budget

    vector<Edge*> added_edges; // sert si le budget est dépassé
	while (budget_left>0 && pccs.size()>0)
	{
		PCC* pcc = pccs.back();
		bool added_complete_pcc = true;

		for (int i = 0; i < pcc->getPath().size(); i++)
		{
			if (budget_left>0)
			{
				Edge* curr_edge = pcc->getPath()[i];
				// on ne recompte pas (pr budget_left) les aretes déjà été aménagées/aretes qui ont le bon lts
				if (curr_edge->get_is_improved() == false && curr_edge->get_edge_LTS() > LTS_max)
				{
					budget_left -= curr_edge->get_edge_cost_1();
					curr_edge->set_is_improved(true);
					added_edges.push_back(curr_edge);
				}
			}
			if (budget_left<0)
			{
                Edge* curr_edge = added_edges.back();
                curr_edge->set_is_improved(false);
                added_edges.pop_back();
                budget_left += curr_edge->get_edge_cost_1();
				added_complete_pcc = false;
				break;
			}
		}
		// si l'on a parcouru ce pcc en entier, on le pop de la liste des pccs pour continuer de la parcourir 
		if (added_complete_pcc)
		{
			pccs.pop_back();
		}
        else { break;} // dès qu'on ne peut pas ajouter une arete, on sort de la boucle
	}
    cout << "budget_left : " << budget_left << endl;
}

// même heuristique, sauf que l'on parcourt tous les arcs des pccs pour remplir le budget au max
void HeuristicPCC::find_edges_to_change_fill_budget()
{
	// 1ere partie : tri de ppcs
	// tri décroissant
	std::sort(pccs.begin(), pccs.end(), myUtils::sortbydecreasdistPCC);
	// cout<<"affichage des pccs apres tri"<<endl;
	// for(int i = 0; i < pccs.size(); i++)
	// {
	// 	cout << *pccs[i] << endl;
	// }

	// tant qu'il y a du budget on aménage le premier pcc et on calcule le nv budget en
    // prenant bien en compte si des aretes ont déjà été prises en compte ds le budget
    vector<Edge*> added_edges; // sert si le budget est dépassé

	while (pccs.size() > 0)
    {
		PCC* pcc = pccs.back();
        pccs.pop_back();
        int i=0;
        for (int i = 0; i < pcc->getPath().size(); i++)
        {
            Edge* curr_edge = pcc->getPath()[i];
            if (budget_left >= curr_edge->get_edge_cost_1() && curr_edge->get_is_improved() == false && curr_edge->get_edge_LTS() > LTS_max)
            {
                budget_left -= curr_edge->get_edge_cost_1();
                curr_edge->set_is_improved(true);
                added_edges.push_back(curr_edge);
            }
            // sinon on skip cette arete et on continue
        }
	}
    cout << "budget_left : " << budget_left << endl;
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

string HeuristicPCC::createFileNameFillBudget()
{
    string filename = "./Results/";
    filename += graph->getGraphName() + "_heuristique_HB_";

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

void HeuristicPCC::compute_objective()
{
    ppoi_barre = 0;
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        int PPOI_visible = curr_tile->getPotentialPoi().size(); // nombre de ppoi visibles
        // cout << "PPOI_visible pour " << curr_tile->getIdTile() << " = " << PPOI_visible << endl;
        for (int p = 0; p < PPOI_visible; p++)
        {
            ppoi_barre++; // le ppoi est visible
            // cout<<"PPOI_var = "<<PPOI_var<<endl;
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            // cout << "z p = " << curr_tile->getIdcentralNode() << " " << poi_ptr->getPoiNode() << endl;
            if (graph->doSecurePathExists(curr_tile->getIdcentralNode(), poi_ptr->getPoiNode(), LTS_max, distance_max)) // voir direction
            {
                // cout << "path exists" << endl;
                ppoi_barre--; // le ppoi est atteint
            }
            // cout<<"PPOI_var = "<<PPOI_var<<endl;
        }
    }
}

void HeuristicPCC::compute_objective_population()
{
    ppoi_barre = 0;
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        long int wz = curr_tile->getTilePopulation();
        int PPOI_visible = curr_tile->getPotentialPoi().size();
        for (int p = 0; p < PPOI_visible; p++)
        {
            ppoi_barre+=wz; // le ppoi est visible
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            if (graph->doSecurePathExists(curr_tile->getIdcentralNode(), poi_ptr->getPoiNode(), LTS_max, distance_max)) // voir direction
            {
                ppoi_barre-=wz; // le ppoi est atteint
            }
        }
    }
}

// cette méthode se base seulement sur les pcc, mais il pourrait y avoir un autre chemin
int HeuristicPCC::compute_objective_with_pccs()
{
    int res = pccs.size();
    for (int i = 0; i < pccs.size(); i++)
    {
        // si le chemin est sur, 
        PCC* pcc = pccs.back(); 
        bool safe_path = true;

		for (int i = 0; i < pcc->getPath().size(); i++)
        {
            Edge* curr_edge = pcc->getPath()[i];
            if ((curr_edge->get_edge_LTS() > LTS_max) && !(curr_edge->get_is_improved()))
            {
                safe_path=false; // on ne peut pas accéder au chemin
                break;
            }
        }
        if (safe_path)
        {
            res--; // le ppoi est atteint
        }
    }
    return res;
}

void HeuristicPCC::solveModel() 
{
    cout << "enter solve" << endl;

    int ov_avant = graph->compute_objective(carreaux, LTS_max, distance_max);
    cout << "objective_value with Graph::compute_objective() AVANT SOLVE = " << ov_avant << endl;

    resolutionTime = 0;
    clock_t start, finish;
    start=clock();
    find_edges_to_change(); // heuristique 
    compute_objective(); // calcul de la solution value (ppoi_barre)
    // compute_objective_population();
    finish=clock();
    resolutionTime = (double)(finish - start) / CLOCKS_PER_SEC;

    cout << "Temps de résolution: " << resolutionTime << endl;
    cout << "Solution value  = " << ppoi_barre << endl;
    cout << "objective_value with Graph::compute_objective()" << ";" << graph->compute_objective(carreaux, LTS_max, distance_max) << endl;

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
 
    resFile << "Heuristique PCC" << endl;
    resFile << "graph_name" << ";" << graph->getGraphName() << endl;
    resFile << "nbTiles" << ";" << carreaux->getNbTiles() << endl;
    resFile << "nbPoi" << ";" << carreaux->getNbPoi() << endl;
    resFile << "nbPoiTileCouple" << ";" << carreaux->getNbPpoiTileCouple() << endl;
    resFile << "objective_value AVANT SOLVE with Graph::compute_objective()" << ";" << ov_avant << endl;

    resFile << "budget" << ";" << budget << endl;
    resFile << "budget_left" << ";" << budget_left << endl;
    resFile << "LTS_max" << ";" << LTS_max << endl;
    resFile << "distance_max" << ";" << distance_max << endl;

    resFile << "modelBuildingTime" << ";" << modelBuildingTime << endl;
    resFile << "resolutionTime" << ";" << resolutionTime << endl;
    resFile << "objective_value" << ";" << ppoi_barre << endl;
    resFile << "objective_value with Graph::compute_objective()" << ";" << graph->compute_objective(carreaux, LTS_max, distance_max) << endl;
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

void HeuristicPCC::solveModelFillBudget() 
{
    cout << "enter solve fill budget" << endl;

    int ov_avant = graph->compute_objective(carreaux, LTS_max, distance_max);
    cout << "objective_value with Graph::compute_objective() AVANT SOLVE = " << ov_avant << endl;

    resolutionTime = 0;
    clock_t start, finish;
    start=clock();
    find_edges_to_change_fill_budget(); // heuristique 
    compute_objective(); // calcul de la solution value (ppoi_barre)
    finish=clock();
    resolutionTime = (double)(finish - start) / CLOCKS_PER_SEC;

    cout << "Temps de résolution: " << resolutionTime << endl;
    cout << "Solution value  = " << ppoi_barre << endl;
    cout << "solution value for pccs = " << compute_objective_with_pccs() << endl;
    cout << "objective_value with Graph::compute_objective()" << ";" << graph->compute_objective(carreaux, LTS_max, distance_max) << endl;

    std::ofstream resFile;
    resFile.open(createFileNameFillBudget(), ios::out);

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
 
    resFile << "Heuristique PCC fill budget" << endl;
    resFile << "graph_name" << ";" << graph->getGraphName() << endl;
    resFile << "nbTiles" << ";" << carreaux->getNbTiles() << endl;
    resFile << "nbPoi" << ";" << carreaux->getNbPoi() << endl;
    resFile << "nbPoiTileCouple" << ";" << carreaux->getNbPpoiTileCouple() << endl;
    resFile << "objective_value AVANT SOLVE with Graph::compute_objective()" << ";" << ov_avant << endl;

    resFile << "budget" << ";" << budget << endl;
    resFile << "budget_left" << ";" << budget_left << endl;
    resFile << "LTS_max" << ";" << LTS_max << endl;
    resFile << "distance_max" << ";" << distance_max << endl;

    resFile << "modelBuildingTime" << ";" << modelBuildingTime << endl;
    resFile << "resolutionTime" << ";" << resolutionTime << endl;
    resFile << "objective_value" << ";" << ppoi_barre << endl;
    resFile << "objective_value with Graph::compute_objective()" << ";" << graph->compute_objective(carreaux, LTS_max, distance_max) << endl;
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
