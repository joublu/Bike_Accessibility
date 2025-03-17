#include "PCC2.h"

using namespace std;

// refaire le constructeur à partir de 
// Graph::find_edges_to_change(Tiles* carreaux, float _b, double _ltsmax, float _dmax)
// un similaire à ModelCplex_BA(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax)

void PCC2::compute_objective()
{
    graph->find_edges_to_change(carreaux, budget, LTS_max, distance_max); // constructeur et heuristique 
    int PPOI_var = 0; // ce que va varier
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        int PPOI_visible = curr_tile->getPotentialPoi().size(); // nombre de ppoi visibles
        for (int p = 0; p < PPOI_visible; p++)
        {
            PPOI_var++; // le ppoi est visible
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            if (graph->doSecurePathExists(z, poi_ptr->getPoiNode(), LTS_max))
            {
                PPOI_var--; // le ppoi est atteint
            }
        }
    }
    ppoi_barre=PPOI_var; // revoir pr utiliser ca directement
}

// mettre budget, LTS_max et distance_max dans la classe
string PCC2::createFileName()
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

void PCC2::solveModel() {

    cout << "enter solve" << endl;

    // cout << "=========================================================================" << endl;

    // env.out() << "Solution value = " << cplex.getObjValue() << endl;

    resolutionTime = 0;
    clock_t start, finish;
    start=clock();

    graph->find_edges_to_change(carreaux, budget, LTS_max, distance_max); // constructeur et heuristique 
    int PPOI_var = 0; // ce que va varier
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* curr_tile = carreaux->getListeOfTiles()[z];
        int PPOI_visible = curr_tile->getPotentialPoi().size(); // nombre de ppoi visibles
        // cout << "PPOI_visible pour " << curr_tile->getIdTile() << " = " << PPOI_visible << endl;
        for (int p = 0; p < PPOI_visible; p++)
        {
            PPOI_var++; // le ppoi est visible
            POI* poi_ptr = curr_tile->getPotentialPoi()[p];
            // cout << "z p = " << curr_tile->getIdcentralNode() << " " << poi_ptr->getPoiNode() << endl;
            if (graph->doSecurePathExists(curr_tile->getIdcentralNode(), poi_ptr->getPoiNode(), LTS_max)) // voir direction
            {
                // cout << "path exists" << endl;
                PPOI_var--; // le ppoi est atteint
            }
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
            resFile << ni << ";" << nj << endl;
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

    resFile << "modelBuildingTime" << ";" << "N/A" << endl;
    resFile << "resolutionTime" << ";" << resolutionTime << endl;
    resFile << "objective_value" << ";" << ppoi_barre << endl;

    resFile << "arc_to_improve" << "; " << endl;
 
}