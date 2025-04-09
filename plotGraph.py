###########

import matplotlib.pyplot as plt
import os
import csv

base_dir = "Bike_Accessibility/Data_BA/"
dataset="19N_1_reindexed_reindexed"
# nodes_file_new = os.path.join(base_dir, f"{dataset}_reindexed_noeuds.csv")
# arcs_file_new = os.path.join(base_dir, f"{dataset}_reindexed_arcs.csv")
# poi_file_new = os.path.join(base_dir, f"{dataset}_reindexed_poi.csv")
# filosofi_file_new = os.path.join(base_dir, f"{dataset}_reindexed_filosofi.csv")

nodes_file_new = os.path.join(base_dir, f"{dataset}_noeuds.csv")
arcs_file_new = os.path.join(base_dir, f"{dataset}_arcs.csv")
poi_file_new = os.path.join(base_dir, f"{dataset}_poi.csv")
filosofi_file_new = os.path.join(base_dir, f"{dataset}_filosofi.csv")

nodes_dict = {}
with open(nodes_file_new, newline='', encoding='utf-8') as f:
    reader = csv.reader(f, delimiter=';')
    header = next(reader)
    for row in reader:
        node_id = int(row[0])
        x_val = float(row[1])
        y_val = float(row[2])
        nodes_dict[node_id] = (x_val, y_val)

x_vals = [coord[0] for coord in nodes_dict.values()]
y_vals = [coord[1] for coord in nodes_dict.values()]

plt.figure(figsize=(10,10))
plt.scatter(x_vals, y_vals, color='blue', label='Noeuds')
for node_id, (x,y) in nodes_dict.items():
    plt.annotate(str(node_id), (x, y), color='purple', fontsize=12, alpha=0.8)

with open(arcs_file_new, newline='', encoding='utf-8') as f:
    reader = csv.reader(f, delimiter=';')
    header_arcs = next(reader)
    for row in reader:
        start_node = int(row[0])
        end_node = int(row[1])
        if start_node in nodes_dict and end_node in nodes_dict:
            x_start, y_start = nodes_dict[start_node]
            x_end, y_end = nodes_dict[end_node]
            dx = x_end - x_start
            dy = y_end - y_start
            plt.arrow(x_start, y_start, dx, dy, width=0.00001, alpha=0.5, color='gray')
            file_distance = float(row[2])
            file_danger = float(row[3])
            ratio = file_danger / file_distance if file_distance != 0 else float('inf')
            mid_x = (x_start + x_end) / 2
            mid_y = (y_start + y_end) / 2
            label = f"{file_distance:.2f}\nLTS: {ratio:.2f}"
            plt.text(mid_x, mid_y, label, fontsize=8, color='black', alpha=0.8)

poi_points = []
with open(poi_file_new, newline='', encoding='utf-8') as f:
    reader = csv.DictReader(f, delimiter=';')
    for row in reader:
        try:
            poi_node = int(row["id_node"].strip())
        except:
            continue
        if poi_node in nodes_dict:
            poi_points.append((poi_node, nodes_dict[poi_node]))
if poi_points:
    x_poi = [pt[1][0] for pt in poi_points]
    y_poi = [pt[1][1] for pt in poi_points]
    plt.scatter(x_poi, y_poi, color='red', marker='o', s=80, label='POI', alpha=0.5)
    for poi_id, (x,y) in poi_points:
        plt.annotate(str(poi_id), (x, y), color='black', fontsize=12, alpha=0.8)

delegate_points = []
with open(filosofi_file_new, newline='', encoding='utf-8') as f:
    reader = csv.DictReader(f, delimiter=';')
    for row in reader:
        delegate = row["noeud_delegue"].strip()
        try:
            delegate_node = int(delegate)
        except:
            continue
        if delegate_node in nodes_dict:
            delegate_points.append((delegate_node, nodes_dict[delegate_node]))
if delegate_points:
    x_del = [pt[1][0] for pt in delegate_points]
    y_del = [pt[1][1] for pt in delegate_points]
    plt.scatter(x_del, y_del, color='green', marker='s', s=100, label='Noeud délégué', alpha=0.5)
    for node_id, (x,y) in delegate_points:
        plt.annotate(str(node_id), (x, y), color='black', fontsize=12, alpha=0.8)

plt.title(f"Noeuds, arcs, POI et noeuds délégués réindexés pour {dataset}")
plt.xlabel("X")
plt.ylabel("Y")
plt.legend()
plt.show()