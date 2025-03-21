#include "Parser.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

Graph* Parser::parse_nodes_and_edges_file(std::string pathNode, std::string pathEdges, char sep, string name, int nbVariante){
    // Graph variables
    Graph* graph = new Graph();
    graph->setGraphName(name);

    std::vector<Node>& list_nodes = graph->getListOfNodes();
	std::vector<Edge>& list_edges = graph->getListOfEdges();

	// File variables
	ifstream nodesFile, edgesFile;
	string line, value;

	long int idNode=0, nameNode;
	double coordX, coordY;

	long int nodeI, nodeJ;
	double* costVariante = new double[2*nbVariante];

	// Check of existence of file
	nodesFile.open(pathNode, ios::in);
	if (!nodesFile.is_open()){
		cout << "error opening nodes file" << endl;
 		return nullptr;
	}
	edgesFile.open(pathEdges, ios::in);
	if (!edgesFile.is_open()){
		cout << "error opening edges file" << endl;
		return nullptr;
	}

    cout << "reading nodes file " << endl;
	// Read node's file
    // In Jérôme and Alaâ file, nodes are numbered from 0
	getline(nodesFile, line); // delete header
	while (getline(nodesFile, line)){
        std::stringstream ss(line);

        getline(ss, value, sep);
        nameNode = atol(value.c_str());

        getline(ss, value, sep);
        coordX = atof(value.c_str());

        getline(ss, value, sep);
        coordY = atof(value.c_str());

		list_nodes.push_back(Node(nameNode, nameNode, coordX, coordY));
	}
    graph->setNbNodes(list_nodes.size());

	//read edge's file
	//int tab[4];
    cout << "reading edge file" << endl;
    long int idx_edge = 0;
    getline(edgesFile, line); // delete header
	while (getline(edgesFile, line)){
		std::stringstream ss(line);

		getline(ss, value, sep);
        nodeI = atol(value.c_str());

        getline(ss, value, sep);
        nodeJ = atol(value.c_str());

        getline(ss, value, sep);
        costVariante[0] = atof(value.c_str()); //distance
            
        getline(ss, value, sep);
        costVariante[1] = atof(value.c_str()); // danger
        

        /*
		bool alreadyExists = false;
		for (int i = 0; i < list_edges.size(); i++){
			if (list_edges[i].get_node_id_1() == graph->getIdForNameNode(nodeI) && list_edges[i].get_node_id_2() == graph->getIdForNameNode(nodeI))
				alreadyExists = true;
		}
        */
		//if(!alreadyExists)
        list_edges.push_back(Edge(idx_edge, nodeI, nodeJ, costVariante[0], costVariante[1]));
        idx_edge++;
	}
    cout << "populate succ and pred " << endl;
	graph->setNbEdges(list_edges.size());
	graph->populate_successors_precessors();

	delete [] costVariante;

    return graph;
}

Tiles* Parser::parse_filsofi_file(std::string path_filosofi, char sep)
{
    // Graph variables
    Tiles* zones = new Tiles();
    std::vector<Tile*>& list_tiles = zones->getListeOfTiles();

    // File variables
    ifstream filosofiFile;
    string line, value;

    long int nodeDelegue;
    long int id_tile;
    long int tile_pop;

    // Check of existence of file
    filosofiFile.open(path_filosofi, ios::in);
    if (!filosofiFile.is_open()) {
        cout << "error opening filosofi file" << endl;
        return nullptr;
    }


    cout << "reading filosofi file " << endl;
    getline(filosofiFile, line); // delete header
    int id_tiles = 0;
    bool i_est_200 = false;
    int cpt_i_est_200 = 0;
    while (getline(filosofiFile, line)) {
        std::stringstream ss(line);

        getline(ss, value, sep);
        getline(ss, value, sep);
        //i_est_200 = atol(value.c_str());
        //if (i_est_200) cpt_i_est_200++;
        getline(ss, value, sep);
        if (!value.empty())
        {
            getline(ss, value, sep);
            nodeDelegue = atol(value.c_str());
            if (nodeDelegue >= 0)
            {

                getline(ss, value, sep);
                getline(ss, value, sep);
                getline(ss, value, sep);
                tile_pop = atol(value.c_str());
                //cout << "Population tile " << tile_pop << endl;

                list_tiles.push_back(new Tile(id_tiles, nodeDelegue, tile_pop));
                id_tiles++;
            }
        }
    }
    zones->setNbTiles(list_tiles.size());

    return zones;
}

// on modifie le graphe et les carreaux dans cette fonction
void Parser::parse_POI_file(std::string path_poi, char sep, Tiles* carreaux, Graph* graph)
{
    // File variables
    ifstream poiFile;
    string line, value;    

    // Check of existence of file
    poiFile.open(path_poi, ios::in);
    if (!poiFile.is_open()) {
        cout << "error opening nodes file" << endl;
        return;
    }

    cout << "reading poi file " << endl;
    getline(poiFile, line); // delete header
    int idPoi = 0;
    int poi_node;
    while (getline(poiFile, line)) {
        std::stringstream ss(line);

        getline(ss, value, sep);
        getline(ss, value, sep);
        getline(ss, value, sep);
        getline(ss, value, sep);
        poi_node = atol(value.c_str());

        // Ajouter le POI à la liste globale des POI
        carreaux->getListOfPoi().push_back(new POI(idPoi, poi_node));
        
        //Ajouter le POI dans la liste du noeud auquel il est rattaché
        graph->getListOfNodes()[poi_node].addPoi(carreaux->getListOfPoi().back()); // carreaux->getListOfPoi().back() est le dernier POI créé
        idPoi++;

    }
    carreaux->setNbPoi(idPoi);

    //Affichage
    /*for (int i = 0; i < idPoi; i++)
    {
        cout << "POI id " << carreaux->getListOfPoi()[i]->getPoiId() << " is on node " << carreaux->getListOfPoi()[i]->getPoiNode() << endl;
    }*/
}

/*ODPairs* Parser::parse_pairs_file(Graph* g, std::string pathPair, char sep, int limit) {
    int k=0;
	ODPairs* ODs = new ODPairs();
	ifstream pairsFile;

	// Check existence
	pairsFile.open(pathPair, ios::in);
	if (!pairsFile.is_open()){
		cout << "error opening pairsFile file" << endl;
		return nullptr;
	}

	//read pairs file
	string line, value;
	long int positionLecteur, positionSep, nodeDetected;

	long int indexPath;
	long int origine;
	long int destination;
	double alpha;
	double distance;
	double danger;


	// Delete Header of the document
	getline(pairsFile, line);

	while (getline(pairsFile, line) && k<limit){
		std::stringstream ss(line);

		getline(ss, value, sep);
		indexPath = atol(value.c_str());

		getline(ss, value, sep);
		origine = g->getIdForNameNode(atol(value.c_str()));

		getline(ss, value, sep);
		destination = g->getIdForNameNode(atol(value.c_str()));

		getline(ss, value, sep);
		alpha = atof(value.c_str());

		getline(ss, value, sep);
		distance = atof(value.c_str());

		getline(ss, value, sep);
		danger = atof(value.c_str());

        getline(ss, value, sep);
        vector<long int> chemin_initial;
        positionLecteur = 1;
        positionSep = value.find(',', positionLecteur);
        while (positionSep != int(value.npos)){
            nodeDetected = g->getIdForNameNode(atol((value.substr(positionLecteur, positionSep-positionLecteur)).c_str()));
            chemin_initial.push_back(nodeDetected);
            positionLecteur = positionSep+1;
            positionSep = value.find(',', positionLecteur);
        }
        chemin_initial.push_back(g->getIdForNameNode(atol((value.substr(positionLecteur, value.npos-positionLecteur-1)).c_str())));

        getline(ss, value, sep);
        vector<long int> sommets_visibles;
        positionLecteur = 1;
        positionSep = value.find(',', positionLecteur);
        while (positionSep != int(value.npos)){
            sommets_visibles.push_back(g->getIdForNameNode(atol((value.substr(positionLecteur, positionSep-positionLecteur)).c_str())));
            positionLecteur = positionSep+1;
            positionSep = value.find(',', positionLecteur);
        }
        sommets_visibles.push_back(g->getIdForNameNode(atol((value.substr(positionLecteur, value.npos-positionLecteur-1)).c_str())));

        getline(ss, value, sep);
        vector<long int> PCC;
        positionLecteur = 1;
        positionSep = value.find(',', positionLecteur);
        while (positionSep != int(value.npos)){
            PCC.push_back(g->getIdForNameNode(atol((value.substr(positionLecteur, positionSep-positionLecteur)).c_str())));
            positionLecteur = positionSep+1;
            positionSep = value.find(',', positionLecteur);
        }
        PCC.push_back(g->getIdForNameNode(atol((value.substr(positionLecteur, value.npos-positionLecteur-1)).c_str())));

		k+=1;

		ODs->add_a_pair(indexPath, origine, destination, alpha, distance, danger, chemin_initial, sommets_visibles, PCC);
	}

	return ODs;
}
*/
/*vector<long int> Parser::parse_trace_order(std::string pathTraceOrder, char sep, int limit) {
    vector<long int> newTraceOrder;
	ifstream TOFile;

	// Check existence
	TOFile.open(pathTraceOrder, ios::in);
	if (!TOFile.is_open()){
		cout << "error opening trace_order file" << endl;
		return newTraceOrder;
	}

	string line, value;
    int k=0;

    while (getline(TOFile, line) && k<limit){
        std::stringstream ss(line);

		getline(ss, value, sep);
		newTraceOrder.push_back(atoi(value.c_str()));
    }

    return newTraceOrder;

}


CampagneParameters* Parser::parse_campagne_parameter(std::string pathCampagneParameter, char sep){
    CampagneParameters* allCampagneParameters;
	ifstream CPFile;

	std::string instance_name;
	vector<double> arrayBudget;
	vector<int> arrayNbPath;
	vector<int> arrayBatchSize;
	vector<int> arrayTypeInit;
	vector<int> arrayFuncBudget;
	vector<int> arrayTraceOrder;
	std::stringstream ss;

    // Reading File
	CPFile.open(pathCampagneParameter, ios::in);
    if (!CPFile.is_open()){
		cout << "error opening campagne_parameter file" << endl;
		return allCampagneParameters = new CampagneParameters("");
	}

    string line, value;

    getline(CPFile, line); // remove header
    getline(CPFile, line); // remove detail redirection

    // InstanceName <std::string>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, instance_name);

    // budget <float>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayBudget.push_back(atof(value.c_str()));
    }

    // nbPath <int>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayNbPath.push_back(atol(value.c_str()));
    }

    // batchSize <int>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayBatchSize.push_back(atol(value.c_str()));
    }

    // typeInit <int>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line); // remove proposition
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayTypeInit.push_back(atol(value.c_str()));
    }

    // funcBudget <int>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line); // remove proposition
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayFuncBudget.push_back(atol(value.c_str()));
    }

    // traceOrder <int>
    getline(CPFile, line); // remove blanck line
    getline(CPFile, line); // remove header
    getline(CPFile, line); // remove proposition
    getline(CPFile, line);
    ss = std::stringstream(line);

    while(getline(ss, value, sep)){
        arrayTraceOrder.push_back(atol(value.c_str()));
    }

    // Creating all CampagneParameters possible
    allCampagneParameters = new CampagneParameters(instance_name);

    for (int indiceBudget = 0; indiceBudget < int(arrayBudget.size()); indiceBudget++){
        for (int indiceNbPath = 0; indiceNbPath < int(arrayNbPath.size()); indiceNbPath++){
            for (int indiceBatchSize = 0; indiceBatchSize < int(arrayBatchSize.size()); indiceBatchSize++){
                for (int indiceTypeInit = 0; indiceTypeInit < int(arrayTypeInit.size()); indiceTypeInit++){
                    for (int indiceFuncBudget = 0; indiceFuncBudget < int(arrayFuncBudget.size()); indiceFuncBudget++){
                        for (int indiceTraceOrder = 0; indiceTraceOrder < int(arrayTraceOrder.size()); indiceTraceOrder++){
                            allCampagneParameters->AddBudget(arrayBudget[indiceBudget]);
                            allCampagneParameters->AddNbPath(arrayNbPath[indiceNbPath]);
                            allCampagneParameters->AddBatchSize(arrayBatchSize[indiceBatchSize]);
                            allCampagneParameters->AddTypeInit(arrayTypeInit[indiceTypeInit]);
                            allCampagneParameters->AddTypeFuncBudget(arrayFuncBudget[indiceFuncBudget]);
                            allCampagneParameters->AddTraceOrder(arrayTraceOrder[indiceTraceOrder]);
                        }
                    }
                }
            }
        }
    }

    return allCampagneParameters;
}

*/