// Bike_Accessibility.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.

#include <iostream>
#include "Parser.h"
#include "Graph.h"
#include "Tiles.h"
#include "ModelCplex_BA.h"
#include "HeuristicPCC.h"

using namespace std;

// // MAIN FOR LOCAL TESTS
// int main()
// {
//     std::cout << "Hello World!\n";
//     string dirPath = "./";
//     string instance_name = "500N_0_reindexed";
//     Parser myParser;
//     // LTS_TYPE = 1 pour les instances de Felix (quand se trouve la colonne cyclability dans le fichier arcs.csv)
//     // LTS_TYPE = 0 pour les instances de Jérôme et Alaâ 
//     // Interet du param : le rapport danger/distance n'est pas équivalent entre ces 2 types
//     int LTS_TYPE = 1;
//     Graph* Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1, LTS_TYPE);
//     cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;
//     Tiles* carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
//     cout << "nb carreaux = " << carreaux->getNbTiles() << endl;

//     float budget = 20.;
//     float dmax = 100;
//     float LTS_max = 1.;

//     int model = 0;
//     // cout << "choose model (0 -> cplex, 1 -> PCC heuristic, 2 -> PCC heuristic filling the budget)" << endl;
//     // cin >> model;

//     if (model == 0) {
//         Tours->initialize_tiles_visibility_set(carreaux, dmax);
//         myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
//         cout << "nb POI = " << carreaux->getNbPoi() << endl;
//         carreaux->initialize_reachable_poi_v2();
//         //carreaux->display_carreaux_data();
//         ModelCplex_BA* cplex_model = new ModelCplex_BA(Tours, carreaux, budget, LTS_max, dmax);
//         cplex_model->solveModel(true, false, false);

//         /*cplex_model->changeC13Constraints(1);
//         cplex_model->solveModel(true, false, false);
//         */

//         /*cplex_model->changeBudgetConstraint(20);
//         cplex_model->solveModel(true, false, false);
//         cplex_model->changeBudgetConstraint(0);
//         cplex_model->solveModel(true, false, false);*/
//     }
//     else if (model == 1) {
//         Tours->initialize_tiles_visibility_set_h(carreaux, dmax, LTS_max);
//         myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
//         cout << "nb POI = " << carreaux->getNbPoi() << endl;
//         carreaux->initialize_reachable_poi_v2();

//         HeuristicPCC* pcc_model = new HeuristicPCC(Tours, carreaux, budget, LTS_max, dmax);
//         pcc_model->solveModel();

//     }
//     else if (model==2) {
//         Tours->initialize_tiles_visibility_set_h(carreaux, dmax, LTS_max);
//         myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
//         cout << "nb POI = " << carreaux->getNbPoi() << endl;
//         carreaux->initialize_reachable_poi_v2();

//         HeuristicPCC* pcc_model = new HeuristicPCC(Tours, carreaux, budget, LTS_max, dmax);
//         pcc_model->solveModelFillBudget();

//     }
//     else {
//         cout << "wrong input, quitting program" << endl;
//     }
// }


// MAIN FOR CASIMODOT
int main()
{
    std::cout << "Hello World!\n";
    string dirPath = "./";
    string instance_name = "100N_1_reindexed";
    // LTS_TYPE = 1 pour les instances de Felix (quand se trouve la colonne cyclability dans le fichier arcs.csv)
    // LTS_TYPE = 0 pour les instances de Jérôme et Alaâ 
    // Interet du param : le rapport danger/distance n'est pas équivalent entre ces 2 types
    int LTS_TYPE = 1;
    Parser myParser;
    Graph* Tours;
    Tiles* carreaux;
    float dmax, budget, LTS_max;

    struct Parameters {
        float dmax;
        float budget;
        float LTS_max;
    };
    
    // vector<Parameters> paramSet = {
    //     {100, 200, 1},
	// 	{500, 200, 1},
	// 	{1000, 200, 1},
	// 	{500, 500, 1},
	// 	{500, 50, 1},
	// 	{500, 20, 1},
	// 	{500, 200, 1.5},
	// 	{100, 200, 1.5},
	// 	{5000, 200, 1},
	// };

    vector<Parameters> paramSet = {
        {500, 20, 1.5},
	};

    // for each tuple, solve the model for the 3 different methods.
    for (auto params : paramSet)
    {
        dmax   = params.dmax;
        budget = params.budget;
        LTS_max = params.LTS_max;
        cout << "Running instance with: dmax = " << dmax << ", budget = " << budget << ", LTS_max = " << LTS_max << endl;


        // CPLEX
        Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1, LTS_TYPE);
        cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;
        carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
        cout << "nb carreaux = " << carreaux->getNbTiles() << endl;
        Tours->initialize_tiles_visibility_set(carreaux, dmax);
        myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
        cout << "nb POI = " << carreaux->getNbPoi() << endl;
        carreaux->initialize_reachable_poi_v2();

        ModelCplex_BA* cplex_model = new ModelCplex_BA(Tours, carreaux, budget, LTS_max, dmax);
        cplex_model->solveModel(true, false, false);
        delete cplex_model;


        // Heuristic PCC
        Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1, LTS_TYPE);
        cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;
        carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
        cout << "nb carreaux = " << carreaux->getNbTiles() << endl;
        Tours->initialize_tiles_visibility_set_h(carreaux, dmax, LTS_max);
        myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
        cout << "nb POI = " << carreaux->getNbPoi() << endl;
        carreaux->initialize_reachable_poi_v2();

        HeuristicPCC* pcc_model = new HeuristicPCC(Tours, carreaux, budget, LTS_max, dmax);
        pcc_model->solveModel();
        delete pcc_model;

        // // Heuristic PCC filling the budget
        // Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1, LTS_TYPE);
        // cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;
        // carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
        // cout << "nb carreaux = " << carreaux->getNbTiles() << endl;
        // Tours->initialize_tiles_visibility_set_h(carreaux, dmax, LTS_max);
        // myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
        // cout << "nb POI = " << carreaux->getNbPoi() << endl;
        // carreaux->initialize_reachable_poi_v2();

        // HeuristicPCC* pcc_model2 = new HeuristicPCC(Tours, carreaux, budget, LTS_max, dmax);
        // pcc_model2->solveModelFillBudget();
        // delete pcc_model2;
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
