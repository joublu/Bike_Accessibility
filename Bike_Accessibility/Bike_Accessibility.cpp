// Bike_Accessibility.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include "Parser.h"
#include "Graph.h"
#include "Tiles.h"
#include "ModelCplex_BA.h"

using namespace std;

/*
Ordre:
1 création du graphe avec le parser
2 création des carreaux avec le parser
3 initialisation de la visibilité des aretes ds les carreaux 
(initialize_tiles_visibility_set fait appel a compute_reachable_edges_v2)
4 parser des POI
5 initialisation des POI atteignables par les carreaux (initialize_reachable_poi_v2)
6 création du modèle cplex
*/

int main()
{
    std::cout << "Hello World!\n";
   // string dirPath = "C:/Users/trault/source/repos/Bike_Accessibility/Bike_Accessibility";
    string dirPath = "./";
    string instance_name = "Tours";
    Parser myParser;
    Graph* Tours = myParser.parse_nodes_and_edges_file(dirPath + "/Data_BA/" + instance_name + "_noeuds.csv", dirPath + "/Data_BA/" + instance_name + "_arcs.csv" , ';', instance_name, 1);

    cout << " nb nodes = " << Tours->getNbNodes() << " nb edges = " << Tours->getNbEdges() << std::endl;

    Tiles* carreaux = myParser.parse_filsofi_file(dirPath + "/Data_BA/" + instance_name + "_filosofi.csv", ';');
    cout << "nb carreaux = " << carreaux->getNbTiles() << endl;

    float dmax = 70; // devrait etre 5000 pr tours
    Tours->initialize_tiles_visibility_set(carreaux, dmax);

    myParser.parse_POI_file(dirPath + "/Data_BA/" + instance_name + "_poi.csv", ';', carreaux, Tours);
    cout << "nb POI = " << carreaux->getNbPoi() << endl;
    carreaux->initialize_reachable_poi_v2();
    //carreaux->display_carreaux_data();

    // arguments : Graph*, Tiles*, budget, LTS max, distance max
    ModelCplex_BA* cplex_model = new ModelCplex_BA(Tours, carreaux, 2000, 1.5, dmax);

    /*cplex_model->changeC13Constraints(1);
    cplex_model->solveModel(true, false, false);
    */
    
    /*cplex_model->changeBudgetConstraint(20);
    cplex_model->solveModel(true, false, false);
    cplex_model->changeBudgetConstraint(0);
    cplex_model->solveModel(true, false, false);*/
    
    

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
