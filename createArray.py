import os
import csv
from tabulate import tabulate

folder = "Bike_Accessibility/Results/"
table = []

for filename in os.listdir(folder):
    if filename.startswith("Tours_Budget_") and (filename.endswith("_v2.csv") or filename.endswith("_v4.csv")):
        fullpath = os.path.join(folder, filename)

        version = "v2" if "_v2.csv" in filename else "v4"
        budget = None
        LTS_max = None
        resolutionTime = None
        nbPoi = None
        arc_count = 0
        arc_section = False

        with open(fullpath, newline='') as csvfile:
            reader = csv.reader(csvfile, delimiter=';')
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
                elif key.startswith("arc_to_improve"):
                    arc_section = True
                elif arc_section:
                    if any(col.strip() for col in row):
                        arc_count += 1

        table.append([version, budget, dmax, LTS_max, modelBuildingTime, resolutionTime, nbPoiTileCouple, objectiveValue, arc_count])

headers = ["Version", "Budget", "dmax", "LTS_max", "Model Building time", "Resolution Time", "#PPOI", "Objective value", "# Arcs Aménagés"]
print(tabulate(table, headers=headers, tablefmt="grid")),