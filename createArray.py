import os
import csv
from tabulate import tabulate

folder = "Bike_Accessibility/Results/"
dataset="Tours"
raw_table = []

for filename in os.listdir(folder):
    if (filename.startswith(dataset) and filename.endswith("_v4.csv")) or filename.startswith(f"{dataset}_heuristique"):
        fullpath = os.path.join(folder, filename)
        version = "PCC" if f"{dataset}_heuristique_" in filename else "methode exacte"
        
        with open(fullpath, newline='') as csvfile:
            reader = csv.reader(csvfile, delimiter=';')
            budget = None
            dmax = None
            LTS_max = None
            resolutionTime = None
            nbPoiTileCouple = None
            objectiveValue = None
            
            for row in reader:
                if not row or row[0].strip() == "":
                    continue
                key = row[0].strip()
                if key == "budget":
                    budget = row[1].strip()
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
                elif key == "objective_value":
                    objectiveValue = row[1].strip()
            raw_table.append([version, budget, dmax, LTS_max, resolutionTime, nbPoiTileCouple, objectiveValue, modelBuildingTime])
            
groups = {}
for row in raw_table:
    key = (row[1], row[2], row[3])
    if key not in groups:
        groups[key] = {}
    groups[key][row[0]] = row 

final_table = []
headers = ["Budget", "dmax", "LTS_max", "Time Exact", "#PPOI Exact", "Objective Value Exact",
           "Time Heur", "#PPOI Heur", "Objective Value Heur", "time saved", "efficiency"]

for key, versions in groups.items():
    # Only add a row if we have both versions:
    if "methode exacte" in versions and "PCC" in versions:
        row_exact = versions["methode exacte"]
        row_heur  = versions["PCC"]
        
        try:
            res_exact = float(row_exact[4]) + float(row_exact[7])
            res_heur  = float(row_heur[4])
            obj_exact = float(row_exact[6])
            obj_heur  = float(row_heur[6])
            nb_ppoi = int(row_exact[5]) # assume it's almost the same for heur
        except Exception as e:
            res_exact = res_heur = obj_exact = obj_heur = None

        if res_exact is not None and res_heur is not None:
            gain_temps = res_exact - res_heur
        else:
            gain_temps = ""
        # if obj_exact == obj_heur:
        #     efficacite = 100
        if obj_exact and obj_exact != 0 and obj_exact is not None and obj_heur is not None:
            # efficacite = (1 + (obj_exact - obj_heur) / obj_exact)*100
            # efficacite = ((nb_ppoi - (obj_heur-obj_exact)) / nb_ppoi)*100
            efficacite = obj_heur-obj_exact
        else:
            efficacite = ""

        final_table.append([
            row_exact[1],
            row_exact[2],
            row_exact[3],
            row_exact[4],
            row_exact[5],
            row_exact[6],
            row_heur[4],
            row_heur[5],
            row_heur[6],
            gain_temps,
            efficacite
        ])
    else:
        pass

print()
print("table for dataset = ", dataset)
print(tabulate(final_table, headers=headers, tablefmt="grid"))
print()