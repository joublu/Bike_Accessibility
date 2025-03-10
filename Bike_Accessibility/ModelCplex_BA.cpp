#include "ModelCplex_BA.h"


/*std::unordered_map<key_type, size_t> idx_mapping;
idx_mapping[{tile, poi, edge}] = 30;
ou
key_t k;
k.tile = ...
tab[idx_mapping[k]];
*/

using namespace std;

ModelCplex_BA::ModelCplex_BA(Graph* _g, Tiles* _t, float _b, double _ltsmax, float _dmax)
{
	graph = _g;
    carreaux = _t;
    nbZones = carreaux->getNbTiles();
    nbPPOI = carreaux->getNbPoi();
    max_pz_pair = nbZones * nbPPOI;
	budget = _b;
    LTS_max = _ltsmax;
    distance_max = _dmax;
	nbNodes = graph->getNbNodes();
	nbEdges = graph->getNbEdges();

    cout << "\t- nbNodes = " << nbNodes << endl;
    cout << "\t- nbEdges = " << nbEdges << endl;
    cout << "\t- budget = " << budget << " and LTS_max = " << LTS_max  << endl;
    cout << "\t Tiles size var tab value = " << carreaux->getsizeVarTab() << endl;


	cout << "\t- Creation de l'environnement" << endl;
	env = IloEnv();

	cout << "\t- Creation du mod�le" << endl;
	model = IloModel(env);

    cout << "\t- Creation des maps" << endl;
    long int var_idx = 0, couple_var_idx = 0;    
    map_size = carreaux->getsizeVarTab();
    couple_map_size = carreaux->getsizeCoupleVarTab(); // voir si pareil que "NUMBER OF PPOI AND ZONES COUPLE"
    cout << "\t map_size = " << map_size << "  et couple_map_size = " << couple_map_size <<  endl;

    idx_mapping.reserve(map_size);
    couple_idx_mapping.reserve(couple_map_size);

    max_ppoi = 0;

    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* tile_ptr = carreaux->getListeOfTiles()[z];
        int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
        if (nb_ppoi > max_ppoi) max_ppoi = nb_ppoi;

        // getEdgeVisibility est initialisé dans Graph::compute_reachable_edges_v2
        int nb_visible_edges = carreaux->getListeOfTiles()[z]->getEdgeVisibility().size();
        for (int p = 0; p < nb_ppoi; p++)
        {
            POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];

            couple_type ct;
            ct.tile = tile_ptr;
            ct.poi = poi_ptr;
            couple_idx_mapping[ct] = couple_var_idx;
            
            couple_var_idx++;

            for (int e = 0; e < nb_visible_edges; e++)
            {
                Edge* edge_ptr = carreaux->getListeOfTiles()[z]->getEdgeVisibility()[e];
                key_type k;
                k.tile = tile_ptr;
                k.poi = poi_ptr;
                k.edge = edge_ptr;
                idx_mapping[k] = var_idx;
                var_idx++;
            }
        }
    }
    /*map_size = var_idx;
    couple_map_size = couple_var_idx;
    cout << "var_idx = " << var_idx << " couple_var_idx" << couple_var_idx << endl;
    */

   /* for (auto it = idx_mapping.begin(); it != idx_mapping.end(); it++)
    {
        long int ni = (*it).first.edge->get_node_id_1();
        long int nj = (*it).first.edge->get_node_id_2();
        long int z = (*it).first.tile->getIdcentralNode();
        long int p = (*it).first.poi->getPoiNode();
        cout << " var idx " << (*it).second << " _e_" << ni << "_" << nj << "_z_" << z << "_p_" << p << endl;

       
    }*/

    budgetExpr = IloExpr(env);
    cplex = IloCplex(env);
    modelBuildingTime = 0;
    double buildingStartTime = cplex.getCplexTime();    
    cout << "\t- Generation des variables " << endl;
    this->generate_variables_model_v2();
    cout << "\t- Generation de l'objectif " << endl;
    this->createObjective();
   // this->createObjectiveOnDistance();
    cout << "\t- Generation des contraintes " << endl;
    this->generate_constraints();
    double buildingStopTime = cplex.getCplexTime();
    modelBuildingTime = buildingStopTime - buildingStartTime;

    cout << "modelBuildingTime = " << modelBuildingTime << endl;
   
    cout << "\t- Call solve model " << endl;
    this->solveModel(true, false, false);
}

ModelCplex_BA::~ModelCplex_BA() {
    cplex.end();
    model.end();
    env.end();
};

void ModelCplex_BA::generate_variables_model_v2()
{
    // y_ij_z^p, yb_ij_z^p et delta_ij_z^p : les 3 dépendent d'un triplet edge, tile, poi trouvé ds idx_mapping
    // 3 tableaux de longueur map_size=lenght(idx_mapping) sont créées pr stocker ces var

    Delta_var = IloNumVarArray(env, map_size);
    Y_var = IloNumVarArray(env, map_size);
    YB_var = IloNumVarArray(env, map_size);

    for (auto it = idx_mapping.begin(); it != idx_mapping.end(); it++)
    {
        int z = it->first.tile->getIdcentralNode();
        int p = it->first.poi->getPoiNode();
        int ni = it->first.edge->get_node_id_1();
        int nj = it->first.edge->get_node_id_2();

        std::stringstream name;
        name << "Delta_e_" << ni << "_" << nj << "_z_" << z << "_p_" << p;
        Delta_var[it->second] = IloNumVar(env, 0, 1, IloNumVar::Int, name.str().c_str());

        std::stringstream name2;
        name2 << "Y_e_" << ni << "_" << nj << "_z_" << z << "_p_" << p;
        Y_var[it->second] = IloNumVar(env, 0, 1, IloNumVar::Int, name2.str().c_str());

        std::stringstream name3;
        name3 << "YB_e_" << ni << "_" << nj << "_z_" << z << "_p_" << p;
        YB_var[it->second] = IloNumVar(env, 0, 1, IloNumVar::Int, name3.str().c_str());

    }

    SB_var = IloNumVarArray(env, nbEdges);
    size_t e = 0;
    for (auto it = graph->getListOfEdges().begin(); it != graph->getListOfEdges().end(); it++)
    {
        int ni = it->get_node_id_1();
        int nj = it->get_node_id_2();
        std::stringstream name;
        name << "SB_e_" << ni << "_" << nj ;
        SB_var[e] = IloNumVar(env, 0, 1, IloNumVar::Int, name.str().c_str());
        e++;
    }

    D_var = IloNumVarArray(env, couple_map_size);
    PPOI_var = IloNumVarArray(env, couple_map_size);

    for (auto it = couple_idx_mapping.begin(); it != couple_idx_mapping.end(); it++)
    {
        int z = it->first.tile->getIdcentralNode();
        int p = it->first.poi->getPoiNode();

        
        std::stringstream name;
        name << "D_z_" << z << "_p_" << p;
        // IloNumVar(const IloEnv env, IloNum lb=0, IloNum ub=IloInfinity, IloNumVar::Type type=Float, const char * name=0)
        D_var[it->second] = IloNumVar(env, 0,IloInfinity, IloNumVar::Float, name.str().c_str());

        std::stringstream name2;
        name2 << "PPOI_z_" << z << "_p_" << p;
        PPOI_var[it->second] = IloNumVar(env, 0, 1, IloNumVar::Int, name2.str().c_str());

    }

}

void ModelCplex_BA::generate_variables_model()
{
   /* Delta_var = IloNumVarArray(env, map_size);
    Y_var = IloNumVarArray(env, map_size);
    YB_var = IloNumVarArray(env, map_size);
    for (size_t s = 0; s < map_size; s++)
    {
        Delta_var[s] = IloNumVar(env, 0, 1, IloNumVar::Bool);        
        Y_var[s] = IloNumVar(env, 0, 1, IloNumVar::Bool);
        YB_var[s] = IloNumVar(env, 0, 1, IloNumVar::Bool);
    }

    SB_var = IloNumVarArray(env, nbEdges);
    for (size_t e = 0; e < nbEdges; e++)
    {
        SB_var[e] = IloNumVar(env, 0, 1, IloNumVar::Bool);
    }

    D_var = IloNumVarArray(env, couple_map_size);
    PPOI_var = IloNumVarArray(env, couple_map_size);

    for (size_t c = 0; c < couple_map_size; c++)
    {
        D_var[c] = IloNumVar(env, 0, IloInfinity, IloNumVar::Float);

        PPOI_var[c] = IloNumVar(env, 0, max_ppoi, IloNumVar::Int);
    }
    */
}

void ModelCplex_BA::generate_constraints()
{
    // C_12 : budget max
    // C_4 et C_5 : SB_var >= YB_var et 1 - SB_var >= Y
    cout << "\t\t- building C4, C5 and C12 " << endl;
    for (size_t e = 0; e < nbEdges; e++)
    {
        // r�cup�rer le edge � partir de son id : id == position dans le vecteur des edges du graph
        Edge* edge_ptr = graph->getGivenEdge(e);
        budgetExpr += SB_var[e] * edge_ptr->get_edge_cost_1();
       // cout << " ++++++++++ edge " << edge_ptr->get_node_id_1() << " to " << edge_ptr->get_node_id_2() << endl;

        // R�cup�rer tous les Y qui contiennent l'edge
        for (int z = 0; z < carreaux->getNbTiles(); z++)
        {
            Tile* tile_ptr = carreaux->getListeOfTiles()[z];
            int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
            for (int p = 0; p < nb_ppoi; p++)
            {
                POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];

                // Construction d'un key_type qui peut exister ou pas
                key_type k;
                k.tile = tile_ptr;
                k.poi = poi_ptr;
                k.edge = edge_ptr;
                
               // cout << " C4 C5 e_" << edge_ptr->get_node_id_1() << "_" << edge_ptr->get_node_id_2() << "_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode() << endl;

                std::unordered_map<key_type, size_t>::const_iterator got = idx_mapping.find(k);
                if (got != idx_mapping.end()) // par def de find
                {
                   // cout << "FOUND  Edge " << got->first.edge->get_node_id_1() << " to "<<  got->first.edge->get_node_id_2() << " tile " << got->first.tile->getIdcentralNode() << " poi " << got->first.poi->getPoiNode() << endl;
                   // cout << " Cst NAME tile "<< tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode()<< endl;
                   // cout << " GOT idx = " << got->second << endl;

                    std::stringstream nameConstraint;
                    IloConstraint c4(SB_var[e] >= YB_var[got->second]);
                    nameConstraint << "constraint_4_(" << e << ")_z_"<< tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
                    c4.setName(nameConstraint.str().c_str());
                    model.add(c4);

                    std::stringstream nameConstraint2;
                    IloConstraint c5(1 - SB_var[e] >= Y_var[got->second]);
                    nameConstraint2 << "constraint_5_(" << e << ")_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
                    c5.setName(nameConstraint2.str().c_str());
                    model.add(c5);
                }
            }
        }

    }  

    this->createBudgetConstraint();

    int cpt_nz_is_np = 0;
    

    //C_3 : Y + YB <= 1 somme
    //C_6 : D = Y + YB
    cout << "\t\t- building C3, C6  " << endl;
    for(auto ite = idx_mapping.begin(); ite != idx_mapping.end(); ite++)
    //for (size_t s = 0; s < map_size; s++)
    {
        size_t var_idx = (*ite).second;
        long int z = (*ite).first.tile->getIdcentralNode();
        long int p = (*ite).first.poi->getPoiNode();
        long int ni = (*ite).first.edge->get_node_id_1();
        long int nj = (*ite).first.edge->get_node_id_2();

        std::stringstream nameConstraint;
        IloConstraint c3(Y_var[var_idx] + YB_var[var_idx] <= 1);
        nameConstraint << "constraint_3_e_" << ni << "_" << nj << "_z_" << z << "_p_" << p;
        c3.setName(nameConstraint.str().c_str());
        model.add(c3);

        std::stringstream nameConstraint2;
        IloConstraint c6(Delta_var[var_idx] == Y_var[var_idx] + YB_var[var_idx]);
        nameConstraint2 << "constraint_6_e_" << ni << "_" << nj << "_z_" << z << "_p_" << p;
        c6.setName(nameConstraint2.str().c_str());
        model.add(c6);        
    }

    // C_7 and C_8
    cout << "\t\t- building C7, C8  " << endl;
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        

        Tile* tile_ptr = carreaux->getListeOfTiles()[z];

        long int node_delegue = carreaux->getListeOfTiles()[z]->getIdcentralNode();
        // r�cup�rer les successeur du noeud central
        std::vector<Edge*>& succ_edge_of_nz = graph->getEdgeSucc(node_delegue);
        std::vector<Edge*> keep_succ_edges;


        // V�rifier que les edge retourn�s sont dans la visibilit� de z car ce sont les successeurs � l'echelle du graphe
        // ne garder que les succ dans la visibilit� dans keep_edges
        for (auto it = succ_edge_of_nz.begin(); it != succ_edge_of_nz.end(); it++)
        {
            if (find(carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin(),
                carreaux->getListeOfTiles()[z]->getEdgeVisibility().end(), *it) != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end())
            {
                keep_succ_edges.push_back(*it);
            }
        }

        int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
        // Pour chaque POI
        for (int p = 0; p < nb_ppoi; p++)
        {
            IloExpr Delta_np(env);
            IloExpr Delta_nz(env);



            POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];
            long int node_poi_np = poi_ptr->getPoiNode();
           
                // r�cup�rer les predecesseurs du noeud du poi
                std::vector<Edge*>& pred_edge_of_np = graph->getEdgePred(node_poi_np);
                //cout << "the list of predessors"
                std::vector<Edge*> keep_pred_edges;

                //cout << "#########################################" << endl;
                //cout << " Noeud delegue = " << node_delegue << " et poi = " << node_poi_np << " dont les predecesseurs du poi sont " << endl;


                // verfier que les edges retourn�s sont la visibilite de z
                for (auto it = pred_edge_of_np.begin(); it != pred_edge_of_np.end(); it++)
                {
                    //cout << "(" << (*it)->get_node_id_1() << "_" << (*it)->get_node_id_2() << ")";
                    if (find(carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin(),
                        carreaux->getListeOfTiles()[z]->getEdgeVisibility().end(), *it) != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end())
                    {
                        keep_pred_edges.push_back(*it);
                        //cout << " in visibility" << endl;
                    }
                   // cout << endl;
                }

                //bool does_np_exist = false;
                //bool does_nz_exist = false;
                //cout << "LES PREDECESEURS DE " << node_poi_np << " sont " << endl;
                for (size_t ke = 0; ke < keep_pred_edges.size(); ke++)
                {
                    Edge* edge_ptr = keep_pred_edges[ke];
                    key_type k;
                    k.tile = tile_ptr;
                    k.poi = poi_ptr;
                    k.edge = edge_ptr;
                    auto it = idx_mapping.find(k);
                    if (it == idx_mapping.end()) {
                        std::cout << "OOOOOPPPPSSSS!!!" << std::endl;
                    }
                    size_t position = it->second;
                    //cout << " ni = " << edge_ptr->get_node_id_1() << " nj = " << edge_ptr->get_node_id_2() << " zone = " << tile_ptr->getIdcentralNode();
                    //cout << " poi = " << poi_ptr->getPoiNode() << endl;
                    //cout << " position de l'edge dans le tableau de var " << position << endl;
                   // does_np_exist = true;
                    Delta_np += Delta_var[position];
                    /*if (z == 1 && p == 0)
                    {
                        cout << " Delta_np " << ke;
                    }*/
                }
                //cout << "FIn des predecesseurs " << endl;


                for (size_t ke = 0; ke < keep_succ_edges.size(); ke++)
                {
                    Edge* edge_ptr = keep_succ_edges[ke];
                    key_type k;
                    k.tile = tile_ptr;
                    k.poi = poi_ptr;
                    k.edge = edge_ptr;
                    size_t position = idx_mapping[k];

                    Delta_nz += Delta_var[position];
                }

                if (tile_ptr->getIdcentralNode() != node_poi_np)
                {

                    std::stringstream nameConstraintC8;
                    IloConstraint c8(Delta_np == 1);
                    nameConstraintC8 << "constraint_8_z_" << tile_ptr->getIdcentralNode() << "_p_" << node_poi_np;
                    c8.setName(nameConstraintC8.str().c_str());
                    model.add(c8);


                    std::stringstream nameConstraintC7;
                    IloConstraint c7(Delta_nz == 1);
                    nameConstraintC7 << "constraint_7_z_" << tile_ptr->getIdcentralNode() << "_p_" << node_poi_np;
                    c7.setName(nameConstraintC7.str().c_str());
                    model.add(c7);
                }
                else
                {
                    cpt_nz_is_np++;
                    // Si nz == np, le POI est toujorus atteignable
                   /* couple_type ct;
                    ct.tile = tile_ptr;
                    ct.poi = poi_ptr;
                    size_t pos = couple_idx_mapping[ct];

                    std::stringstream nameConstraintAdd;
                    IloConstraint cnot78(PPOI_var[pos] == 1);
                    nameConstraintAdd << "constraint_insteadof78_z_" << tile_ptr->getIdcentralNode() << "_p_" << node_poi_np;
                    cnot78.setName(nameConstraintAdd.str().c_str());
                    model.add(cnot78);

                    std::stringstream nameConstraintnot78bis;
                    IloConstraint cnot78bis(D_var[pos] == 1);
                    nameConstraintnot78bis << "constraint_insteadof78bis_z_" << tile_ptr->getIdcentralNode() << "_p_" << node_poi_np;
                    cnot78bis.setName(nameConstraintnot78bis.str().c_str());
                    model.add(cnot78bis);
                    */
                }
            

        }
    }

    cout << "cpt_nz_is_np = " << cpt_nz_is_np << endl;

    //C_9 C_10 C_11
    cout << "\t\t- building C9 C10 C11 " << endl;
        for (int z = 0; z < carreaux->getNbTiles(); z++)
        {
            Tile* tile_ptr = carreaux->getListeOfTiles()[z];
            long int node_delegue = carreaux->getListeOfTiles()[z]->getIdcentralNode();
            int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
            // Pour chauqe noeuds j dans la visibilite de z
            for (auto it = carreaux->getListeOfTiles()[z]->getNodeVisibility().begin(); it != carreaux->getListeOfTiles()[z]->getNodeVisibility().end(); it++)
            {
                bool is_nz_or_np = false;
                // On ne considere pas les nz ni les np (noeuds delegues et poi)
                if ((*it)->getId() == node_delegue)  is_nz_or_np = true;

                long int curentNodej = (*it)->getId();
                
                for (auto it_poi = carreaux->getListeOfTiles()[z]->getPotentialPoi().begin();
                    it_poi != carreaux->getListeOfTiles()[z]->getPotentialPoi().end(); it_poi++)
                {
                    if ((*it_poi)->getPoiNode() == (*it)->getId())
                    {
                        is_nz_or_np = true;
                        break; // le noeud est un np
                    }
                }
                //if (!is_nz_or_np)
                {
                     // r�cup�rer les succ et pred et ne garder que les edges qui sont dans la visibilit�
                    // r�cup�rer les predecesseurs du noeud du poi
                    std::vector<Edge*>& pred_edge_of_j = graph->getEdgePred((*it)->getId());
                    std::vector<Edge*> keep_pred_edges;

                    std::vector<Edge*>& succ_edge_of_j = graph->getEdgeSucc((*it)->getId());
                    std::vector<Edge*> keep_succ_edges;

                    // verfier que les edges retourn�s sont la visibilite de z
                    for (auto itpred = pred_edge_of_j.begin(); itpred != pred_edge_of_j.end(); itpred++)
                    {
                        if (find(carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin(),
                            carreaux->getListeOfTiles()[z]->getEdgeVisibility().end(), *itpred) != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end())
                        {
                            keep_pred_edges.push_back(*itpred);
                        }
                    }

                    for (auto itsucc = succ_edge_of_j.begin(); itsucc != succ_edge_of_j.end(); itsucc++)
                    {
                        // find is find first matching *itsucc
                        if (find(carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin(),
                            carreaux->getListeOfTiles()[z]->getEdgeVisibility().end(), *itsucc) != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end())
                        {
                            keep_succ_edges.push_back(*itsucc);
                        }
                    }
                                        
                    for (int p = 0; p < nb_ppoi; p++)
                    {
                        POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];

                        IloExpr sum_pred_j(env);
                        IloExpr sum_succ_j(env);
                        
                        bool does_pred_exist = false;
                        bool does_succ_exist = false;

                        for (size_t ke = 0; ke < keep_pred_edges.size(); ke++)
                        {
                            Edge* edge_ptr = keep_pred_edges[ke];

                            key_type k;
                            k.tile = tile_ptr;
                            k.poi = poi_ptr;
                            k.edge = edge_ptr;
                            size_t position = idx_mapping[k];
                            sum_pred_j += Delta_var[position];
                            does_pred_exist = true;
                        }


                        for (size_t ke = 0; ke < keep_succ_edges.size(); ke++)
                        {
                            Edge* edge_ptr = keep_succ_edges[ke];
                            key_type k;
                            k.tile = tile_ptr;
                            k.poi = poi_ptr;
                            k.edge = edge_ptr;
                            size_t position = idx_mapping[k];
                            sum_succ_j += Delta_var[position];
                            does_succ_exist = true;
                        }
                        
                        if (does_pred_exist)
                        {
                            std::stringstream nameConstraintC10;
                            IloConstraint c10(sum_pred_j <= 1);
                            nameConstraintC10 << "constraint_10_NODE_" << curentNodej << "_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
                            c10.setName(nameConstraintC10.str().c_str());
                            model.add(c10);
                        }
                        if (does_succ_exist)
                        {
                            std::stringstream nameConstraintC11;
                            IloConstraint c11(sum_succ_j <= 1);
                            nameConstraintC11 << "constraint_11_NODE_" << curentNodej << "_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
                            c11.setName(nameConstraintC11.str().c_str());
                            model.add(c11);
                        }

                        if (!is_nz_or_np && (does_succ_exist || does_pred_exist))
                        {
                            std::stringstream nameConstraintC9;
                            IloConstraint c9(sum_pred_j == sum_succ_j);
                            nameConstraintC9 << "constraint_9_NODE_" << curentNodej  << "_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
                            c9.setName(nameConstraintC9.str().c_str());
                            model.add(c9);
                        }

                    }

                }
            }
        }//end for each tile

    cout << "\t\t- building C14 C15 " << endl;
    //long int BIG_M = std::numeric_limits<double>::infinity();
    long int BIG_M = 20000;
    size_t c = 0;
    for(auto itt = couple_idx_mapping.begin(); itt != couple_idx_mapping.end(); itt++)
    {
       
        long int z = itt->first.tile->getIdcentralNode();
        long int p = itt->first.poi->getPoiNode();

        c = itt->second;

        std::stringstream nameConstraintC14;
        IloConstraint c14(PPOI_var[c] <= D_var[c] / distance_max);
        nameConstraintC14 << "constraint_14_c_" << c << "_z_" << z << "_p_" << p;
        c14.setName(nameConstraintC14.str().c_str());
        model.add(c14);

        std::stringstream nameConstraintC15;
        IloConstraint c15(PPOI_var[c] >= -(distance_max - D_var[c]) / BIG_M);
        nameConstraintC15 << "constraint_15_c_" << c << "_z_" << z << "_p_" << p;
        c15.setName(nameConstraintC15.str().c_str());
        model.add(c15);

        
    }
    
    cout << "\t\t- building C13" << endl;
    c13Constraints = IloConstraintArray(env);
    // R�cup�rer tous les Y qui contiennent l'edge
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* tile_ptr = carreaux->getListeOfTiles()[z];
        int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
        for (int p = 0; p < nb_ppoi; p++)
        {
            POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];

            sum_edges_costs = IloExpr(env); 

            couple_type ct;
            ct.tile = tile_ptr;
            ct.poi = poi_ptr;
            size_t d_var_position = couple_idx_mapping[ct];

            for (auto itvisibleedge = carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin();
                itvisibleedge != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end();
                itvisibleedge++)
            {
                Edge* edge_ptr = *itvisibleedge;
                key_type k;
                k.tile = tile_ptr;
                k.poi = poi_ptr;
                k.edge = edge_ptr;
                size_t Y_YB_var_position = idx_mapping[k];  

                // Differencier les arcs que l'on peut amm�nager !!!! >> faire depndre les couts ?                
                float cost1_after_improvement = edge_ptr->get_edge_cost_1();
                // Le cout avant am�nagement est mis tr�s grand pour tous les arcs
                float cost1 = distance_max + 1;//  cost1_after_improvement * 2;
                // Si l'arc a un LTS inf ou egal a celui vis�, le cout avant et apres est le m�me, c'est la distance
                if (edge_ptr->get_edge_LTS() <= LTS_max)
                {
                    cost1 = edge_ptr->get_edge_cost_1(); 

                    //            // Si l'arc est am�nag�, mettre les YB � zero ?????
                    //cout << " edge" << edge_ptr->get_node_id_1() << " - " << edge_ptr->get_node_id_2()  <<" is already improved " << endl;
                    //model.add(YB_var[Y_YB_var_position] == 0);
                    c13Constraints.add(YB_var[Y_YB_var_position] == 0);
                }
        
                sum_edges_costs += Y_var[Y_YB_var_position] * cost1 + YB_var[Y_YB_var_position] * cost1_after_improvement;
            }

            std::stringstream nameConstraintC13;
            IloConstraint c13(D_var[d_var_position] == sum_edges_costs);
            nameConstraintC13 << "constraint_13_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
            c13.setName(nameConstraintC13.str().c_str());
            c13Constraints.add(c13);
            //model.add(c13);

            

        }
       
    }
    model.add(c13Constraints);
       
}


void ModelCplex_BA::createObjective() 
{
    objectiveExpr = IloExpr(env);
    for (size_t c = 0; c < couple_map_size; c++) {
        objectiveExpr += PPOI_var[c];
    }
    objectiveVariable = IloMinimize(env, objectiveExpr);
    model.add(objectiveVariable);
}

void ModelCplex_BA::createObjectiveOnDistance()
{
    objectiveExpr = IloExpr(env);
    for (size_t c = 0; c < couple_map_size; c++) {
        objectiveExpr += D_var[c];
    }
    objectiveVariable = IloMinimize(env, objectiveExpr);
    model.add(objectiveVariable);
}

void ModelCplex_BA::solveModel(bool affichage, bool needExport, bool setOffPreSolve) {

    cout << "enter solve" << endl;
    cplex.extract(model);
    //cplex.setParam(IloCplex::Param::Threads, 1);    
    //cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, 0.0001);
    //cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 0.00001);
    //cplex.exportModel("esay.lp");

    if (!affichage)// Block output on console
        cplex.setOut(env.getNullStream());
    else
        cout << endl;

    if (setOffPreSolve)
         cplex.setParam(IloCplex::Param::Preprocessing::Presolve, false);

     //feasibleSolutionTime = -1;
     // Utilisation d'un InfoCallBack pour rechercher le temps de pour trouver une solution faisable, la solution optimalee et le temps pour prouver l'optimalit�
    // cplex.use(GetTimeEtapeResolution(env, &bestObjectiveValue, &feasibleSolutionTime, &optimumSolutionTime));

    cout << "begin to solve" << endl;

    resolutionTime = 0;
    double startTime = cplex.getCplexTime();
    cplex.setParam(IloCplex::Param::TimeLimit, 36000);
    bool res = cplex.solve();
    double stopTime = cplex.getCplexTime();
    resolutionTime = stopTime - startTime;
    cout << "Temps de r�solution: " << resolutionTime << endl;

    if (res) {
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;

        std::ofstream resFile;
        resFile.open(createFileName(), ios::out);
        if ( !resFile.is_open())
        {
            std::cout << "error opening results file" << endl;
            return;
        }

        std::ofstream GLOBALResFile;
        GLOBALResFile.open("./Results/global.csv", ios::app); // revoir nom pr qu'il soit unique etc (sinn ça pose des erreurs à l'execution)
        // if (!GLOBALResFile.is_open())
        // {
        //     std::cout << "error opening GLOBALResFile" << endl;
        //     return;
        // }
        // GLOBALResFile << graph->getGraphName() << ";" << carreaux->getNbTiles() << ";" << carreaux->getNbPoi() << ";" << carreaux->getNbPpoiTileCouple() << ";";
        // GLOBALResFile << budget << ";" << LTS_max << ";" << distance_max << ";" << modelBuildingTime << ";" << resolutionTime << ";" << cplex.getObjValue() << ";";

        resFile << "graph_name" << ";" << graph->getGraphName() << endl;
        resFile << "nbTiles" << ";" << carreaux->getNbTiles() << endl;
        resFile << "nbPoi" << ";" << carreaux->getNbPoi() << endl;
        resFile << "nbPoiTileCouple" << ";" << carreaux->getNbPpoiTileCouple() << endl;

        resFile << "budget" << ";" << budget << endl;
        resFile << "LTS_max" << ";" << LTS_max << endl;
        resFile << "distance_max" << ";" << distance_max << endl;

        resFile << "modelBuildingTime" << ";" << modelBuildingTime << endl;
        resFile << "resolutionTime" << ";" << resolutionTime << endl;
        resFile << "objective_value" << ";" << cplex.getObjValue() << endl;

        /*for (auto it = couple_idx_mapping.begin(); it != couple_idx_mapping.end(); it++)
        {
            int z = it->first.tile->getIdcentralNode();
            int p = it->first.poi->getPoiNode();           


            cout << "D_z_" << z << "_p_" << p << " is equal to " <<    cplex.getValue(D_var[it->second]) << endl;
            cout << "PPOI_z_" << z << "_p_" << p << " is equal to " << cplex.getValue(PPOI_var[it->second]) << endl;
            cout << "path is going through : ";
            // Display the path
            for (auto edge_it = it->first.tile->getEdgeVisibility().begin(); edge_it != it->first.tile->getEdgeVisibility().end(); edge_it++)
            {
                key_type k;
                k.tile = it->first.tile;
                k.poi = it->first.poi;
                k.edge = (*edge_it);

                size_t position = idx_mapping[k];
                if (cplex.getValue(Delta_var[position]) > 0.5)
                    cout << "( " << (*edge_it)->get_node_id_1() << " - " << (*edge_it)->get_node_id_2() << " ) ";
            }
            cout << endl;

        }    
        */
        resFile << "arc_to_improve" << "; " << endl;
        long int cpt_arcs_amenages = 0;
        size_t e = 0;
        for (auto it = graph->getListOfEdges().begin(); it != graph->getListOfEdges().end(); it++)
        {
            
            int ni = it->get_node_id_1();
            int nj = it->get_node_id_2();
            if (cplex.getValue(SB_var[e]) >= 0.5)
            {
                cout << "SB_e_" << ni << "_" << nj << " is improved" << endl;
                resFile << ni << ";" << nj << endl;
                cpt_arcs_amenages++;
            }
            e++;
        }
        // GLOBALResFile << cpt_arcs_amenages << ";" << endl;
        // cout << "finished : cpt_arcs_amenages =  " << cpt_arcs_amenages << endl;
    }
    else
    {
        cout << " no solution " << endl;
    }
   
}

void ModelCplex_BA::createBudgetConstraint()
{
    cout << "enter  createBudgetConstraint" << endl;
    budgetConstraint = IloConstraint(budgetExpr <= budget);
    budgetConstraint.setName("constraint_12_budget");
    model.add(budgetConstraint);
}

void ModelCplex_BA::changeBudgetConstraint(float _newBudget)
{
    cout << "enter changeBudgetConstraint" << endl;
    if (_newBudget < budget)
    {
        this->budget = _newBudget;
        this->createBudgetConstraint();
    }
    else
    {
        cout << "error newbudget is higher than previous budget" << endl;
        return;
    }
    //model.remove(budgetConstraint);
    
}

void ModelCplex_BA::changeC13Constraints(double newLTSmax)
{
    model.remove(c13Constraints);
    this->LTS_max = newLTSmax;

    cout << "\t\t- building C13" << endl;
    c13Constraints = IloConstraintArray(env);
    // R�cup�rer tous les Y qui contiennent l'edge
    for (int z = 0; z < carreaux->getNbTiles(); z++)
    {
        Tile* tile_ptr = carreaux->getListeOfTiles()[z];
        int nb_ppoi = carreaux->getListeOfTiles()[z]->getPotentialPoi().size();
        for (int p = 0; p < nb_ppoi; p++)
        {
            POI* poi_ptr = carreaux->getListeOfTiles()[z]->getPotentialPoi()[p];

            sum_edges_costs = IloExpr(env);

            couple_type ct;
            ct.tile = tile_ptr;
            ct.poi = poi_ptr;
            size_t d_var_position = couple_idx_mapping[ct];

            for (auto itvisibleedge = carreaux->getListeOfTiles()[z]->getEdgeVisibility().begin();
                itvisibleedge != carreaux->getListeOfTiles()[z]->getEdgeVisibility().end();
                itvisibleedge++)
            {
                Edge* edge_ptr = *itvisibleedge;
                key_type k;
                k.tile = tile_ptr;
                k.poi = poi_ptr;
                k.edge = edge_ptr;
                size_t Y_YB_var_position = idx_mapping[k];

                // Differencier les arcs que l'on peut amm�nager !!!! >> faire depndre les couts ?                
                float cost1_after_improvement = edge_ptr->get_edge_cost_1();
                // Le cout avant am�nagement est mis tr�s grand pour tous les arcs
                float cost1 = distance_max + 1;//  cost1_after_improvement * 2;
                // Si l'arc a un LTS inf ou egal a celui vis�, le cout avant et apres est le m�me, c'est la distance
                if (edge_ptr->get_edge_LTS() <= LTS_max)
                {
                    cost1 = edge_ptr->get_edge_cost_1();

                    //            // Si l'arc est am�nag�, mettre les YB � zero ?????
                    //cout << " edge" << edge_ptr->get_node_id_1() << " - " << edge_ptr->get_node_id_2()  <<" is already improved " << endl;
                    //model.add(YB_var[Y_YB_var_position] == 0);
                    c13Constraints.add(YB_var[Y_YB_var_position] == 0);
                }

                sum_edges_costs += Y_var[Y_YB_var_position] * cost1 + YB_var[Y_YB_var_position] * cost1_after_improvement;
            }

            std::stringstream nameConstraintC13;
            IloConstraint c13(D_var[d_var_position] == sum_edges_costs);
            nameConstraintC13 << "constraint_13_z_" << tile_ptr->getIdcentralNode() << "_p_" << poi_ptr->getPoiNode();
            c13.setName(nameConstraintC13.str().c_str());
            c13Constraints.add(c13);
            //model.add(c13);
        }
    }
    model.add(c13Constraints);
}








string ModelCplex_BA::createFileName()
{
    string filename = "./Results/";
    filename += graph->getGraphName() + "_";

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
