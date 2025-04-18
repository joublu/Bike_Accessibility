import os
import csv
from tabulate import tabulate

folder = "Bike_Accessibility/Results"
dataset="19N_1_reindexed_reindexed"
raw_table = []

for filename in os.listdir(folder):
    if (filename.startswith(dataset)):
        fullpath = os.path.join(folder, filename)

        if f"{dataset}_heuristique_HB" in filename:
            version="hb"
        elif f"{dataset}_heuristique" in filename:
            version="h"

        elif f"{dataset}_ME_v4" in filename:
            version="v4"
        elif f"{dataset}_ME_v3" in filename:
            version="v3"

        else:
            version="trash"
        
        with open(fullpath, newline='') as csvfile:
            reader = csv.reader(csvfile, delimiter=';')
            budget = None
            budget_left = None
            dmax = None
            LTS_max = None
            resolutionTime = None
            modelBuildingTime = None
            nbPoiTileCouple = None
            objectiveValue = None
            arc_count = 0
            arc_section = False
            
            for row in reader:
                if not row or row[0].strip() == "":
                    continue
                key = row[0].strip()
                if key == "budget":
                    budget = row[1].strip()
                if key == "budget_left" or key == "budget left":
                    budget_left = row[1].strip()
                if key == "distance_max":
                    dmax = row[1].strip()
                elif key == "LTS_max":
                    LTS_max = row[1].strip()
                elif key == "modelBuildingTime":
                    modelBuildingTime = row[1].strip()
                elif key == "resolutionTime":
                    resolutionTime = row[1].strip()
                elif key == "nbPoiTileCouple":
                    nbPoiTileCouple = row[1].strip()
                elif key == "objective_value AVANT SOLVE with Graph::compute_objective()":
                    objectiveValueIni = row[1].strip()
                elif key == "objective_value with Graph::compute_objective()":
                    OV2 = row[1].strip()
                elif key == "objective_value":
                    objectiveValue = row[1].strip()
                elif key.startswith("arc_to_improve"):
                    arc_section = True
                elif arc_section:
                    if any(col.strip() for col in row):
                        arc_count += 1
            raw_table.append([version, budget, dmax, LTS_max, resolutionTime, nbPoiTileCouple, objectiveValue, modelBuildingTime,arc_count, budget_left,objectiveValueIni,OV2])
            
groups = {}
for row in raw_table:
    key = (row[1], row[2], row[3])
    if key not in groups:
        groups[key] = {}
    groups[key][row[0]] = row 

final_table = []
# headers = ["B", "dmax", "lts", "#ppoi\nME","#ppoi\nH","#improved\nedges ME","#improved\nedges H","budget\nleft ME","budget\nleft H", "Obj Val\nME","Obj val\nH","reached\nPOI ME", "reached\nPOI H", "EFFICIENCY", "time\nME", "Time\nH", "TIME\nSAVED"]
# headers = ["B", "dmax", "lts", "#ppoi\nv4","#ppoi\nv3","#improved\nedges v4","#improved\nedges v3","budget\nleft v4","budget\nleft v3", "Obj Val\nv4","Obj val\nv3","reached\nPOI v4", "reached\nPOI H", "EFFICIENCY", "time\nv4", "Time\nv3", "TIME\nSAVED"]
# headers = ["B", "dmax", "lts", "#ppoi", "Obj Val\ncplex v4","Obj val\ncplex v3","Obj Val\nDSPE v4","Obj val\nDSPE v3","reached\nPOI v4", "reached\nPOI v3", "EFFICIENCY\ncplex", "EFFICIENCY\nDSPE", "time\nv4", "Time\nv3", "TIME\nSAVED"]
# headers = ["B", "dmax", "lts", "#ppoi", "budget\nleft ME","budget\nleft h", "Obj Val\ncplex ME","Obj val\ncplex h","Obj Val\nDSPE ME","Obj val\nDSPE h","reached\nPOI ME", "reached\nPOI H", "EFFICIENCY\ncplex", "EFFICIENCY\nDSPE", "time\nME", "Time\nH", "TIME\nSAVED"]
headers = ["B", "dmax", "lts", "#ppoi", "budget\nleft h1","budget\nleft h2", "Obj Val\ncplex h1","Obj val\ncplex h2","Obj Val\nDSPE h1","Obj val\nDSPE h2","reached\nPOI h1", "reached\nPOI h2", "EFFICIENCY\ncplex", "EFFICIENCY\nDSPE", "time\nh1", "Time\nh2", "TIME\nSAVED"]


for key, versions in groups.items():
    # Only add a row if we have all versions needed:
    if "h" in versions and "v4" in versions:
        row_exact = versions["v4"]
        row_heur  = versions["h"]

        try:
            res_exact = float(row_exact[4]) + float(row_exact[7])
            res_heur  = float(row_heur[4]) + float(row_heur[7])
            gain_temps = res_exact - res_heur
        except Exception as e:
            gain_temps = None

        reached_poi_exact = float(row_exact[5]) - float(row_exact[6])
        reached_poi_heur = float(row_heur[5]) - float(row_heur[6])

        try :
            # efficacite = (float(row_exact[5])-float(row_heur[6]))/(float(row_exact[5])-float(row_exact[6]))*100
            efficacite = (reached_poi_heur) / (reached_poi_exact) * 100
        except Exception as e:
            efficacite = None

        try :
            efficacite2 = (float(row_exact[10])-float(row_heur[11]))/(float(row_exact[10])-float(row_exact[11]))*100
        except Exception as e:
            efficacite2 = None
        
        final_table.append([
            row_exact[1], # budget
            row_exact[2], # dmax
            row_exact[3], # lts
            row_exact[5], # nb ppoi exact
            row_exact[9], # budget left exact
            row_heur[9], # budget left heur
            int(float(row_exact[6])), # obj exact
            int(float(row_heur[6])), # obj heur
            int(float(row_exact[11])), # obj exact Graph::compute_objective
            int(float(row_heur[11])), # obj heur Graph::compute_objective
            float(row_exact[5])-float(row_exact[6]), # nb ppoi exact - obj exact (reached ppoi)
            float(row_heur[5])-float(row_heur[6]), # nb ppoi heurs -obj heur
            efficacite,
            efficacite2,
            float(row_exact[4]) + float(row_exact[7]), # time exact
            float(row_heur[4]) + float(row_heur[7]), # time heur
            gain_temps,
        ])
    else:
        pass

print()
print("table for dataset = ", dataset)
# print("model cplex = v4 (visibilité exacte) et heuristique, DSPE = dijsktra et arrêt dès qu'un chemin sécurisé est trouvé") # avec parcourt complet du graphe")
# print("modèle cplex = v4 (visibilité complète) et modèle cplex = v3 (visibilité réduite), DSPE = dijsktra et arrêt dès qu'un chemin sécurisé est trouvé")
# print("heuristiques h1 et h2, DSPE = dijsktra et arrêt dès qu'un chemin sécurisé est trouvé")
print()
print(tabulate(final_table, headers=headers))
print()