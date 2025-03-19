// Bike_Accessibility.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.

#include <iostream>
#include "Parser.h"
#include "Graph.h"
#include "Tiles.h"
#include "ModelCplex_BA.h"
#include "HeuristicPCC.h"

using namespace std;

int main()
{
    std::cout << "Hello World!\n";
    string dirPath = "./";
    string instance_name = "100N_1_reindexed";
    Parser myParser;
    Graph* Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1);
    cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;
    Tiles* carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
    cout << "nb carreaux = " << carreaux->getNbTiles() << endl;

    float budget = 10;
    float dmax = 20;
	float LTS_max = 1;

    int model;
    cout << "choose model (0 -> cplex, 1 -> PCC heuristic)" << endl;
    cin >> model;

    if (model == 0) {
        Tours->initialize_tiles_visibility_set(carreaux, dmax);
        myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
        cout << "nb POI = " << carreaux->getNbPoi() << endl;
        carreaux->initialize_reachable_poi_v2();
        //carreaux->display_carreaux_data();
        ModelCplex_BA* cplex_model = new ModelCplex_BA(Tours, carreaux, budget, LTS_max, dmax);
        cplex_model->solveModel(true, false, false);
        
        /*cplex_model->changeC13Constraints(1);
        cplex_model->solveModel(true, false, false);
        */
        
        /*cplex_model->changeBudgetConstraint(20);
        cplex_model->solveModel(true, false, false);
        cplex_model->changeBudgetConstraint(0);
        cplex_model->solveModel(true, false, false);*/
    }
    else if (model == 1) {
        Tours->initialize_tiles_visibility_set_h(carreaux, dmax);
        myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
        cout << "nb POI = " << carreaux->getNbPoi() << endl;
        carreaux->initialize_reachable_poi_v2();
        HeuristicPCC* pcc_model = new HeuristicPCC(Tours, carreaux, budget, LTS_max, dmax);
        pcc_model->solveModel();
    }
    else {
        cout << "wrong input, quitting program" << endl;
    }
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
