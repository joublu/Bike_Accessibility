'''Script to reindex the nodes in the dataset
OSM indexes become 0, 1, 2, ...'''

import os
import csv

base_dir = "Bike_Accessibility/Data_BA/"
dataset="100N_1"
nodes_file = os.path.join(base_dir, f"{dataset}_noeuds.csv")
arcs_file = os.path.join(base_dir, f"{dataset}_arcs.csv")
poi_file = os.path.join(base_dir, f"{dataset}_poi.csv")
filosofi_file = os.path.join(base_dir, f"{dataset}_filosofi.csv")
nodes_file_new = os.path.join(base_dir, f"{dataset}_reindexed_noeuds.csv")
arcs_file_new = os.path.join(base_dir, f"{dataset}_reindexed_arcs.csv")
poi_file_new = os.path.join(base_dir, f"{dataset}_reindexed_poi.csv")
filosofi_file_new = os.path.join(base_dir, f"{dataset}_reindexed_filosofi.csv")

'''
1 mapping
2 changer file nodes
3 changer file arcs, '''
node_mapping = {}  # mapping: old id -> new index (both str)
nodes_rows = []

with open(nodes_file, newline='', encoding='utf-8') as fin:
    reader = csv.reader(fin, delimiter=';')
    header = next(reader)
    nodes_rows.append(header)
    index = 0
    for row in reader:
        old_id = row[0].strip()
        node_mapping[old_id] = str(index)
        new_row = row.copy()
        new_row[0] = str(index)
        nodes_rows.append(new_row)
        index += 1
# print(node_mapping)
with open(nodes_file_new, 'w', newline='', encoding='utf-8') as fout:
    writer = csv.writer(fout, delimiter=';')
    writer.writerows(nodes_rows)

arcs_rows = []
with open(arcs_file, newline='', encoding='utf-8') as fin:
    reader = csv.reader(fin, delimiter=';')
    header_arcs = next(reader)  # header row
    arcs_rows.append(header_arcs)
    for row in reader:
        row=row[1:]
        print(row)
        old_start = row[0].strip()
        old_end = row[1].strip()
        # Substitute using mapping if possible
        new_start = node_mapping.get(old_start, -1)
        new_end = node_mapping.get(old_end, -1)
        row[0] = new_start
        row[1] = new_end
        arcs_rows.append(row)
with open(arcs_file_new, 'w', newline='', encoding='utf-8') as fout:
    writer = csv.writer(fout, delimiter=';')
    writer.writerows(arcs_rows)

poi_rows = []
with open(poi_file, newline='', encoding='utf-8') as fin:
    reader = csv.DictReader(fin, delimiter=';')
    fieldnames = reader.fieldnames
    for row in reader:
        old_id = row["id_node"].strip()
        row["id_node"] = node_mapping.get(old_id, old_id)
        poi_rows.append(row)
with open(poi_file_new, 'w', newline='', encoding='utf-8') as fout:
    writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=';')
    writer.writeheader()
    writer.writerows(poi_rows)

filosofi_rows = []
with open(filosofi_file, newline='', encoding='utf-8') as fin:
    reader = csv.DictReader(fin, delimiter=';')
    fieldnames_f = reader.fieldnames
    for row in reader:
        old_delegate = row["noeud_delegue"].strip()
        row["noeud_delegue"] = node_mapping.get(old_delegate, old_delegate)
        filosofi_rows.append(row)
with open(filosofi_file_new, 'w', newline='', encoding='utf-8') as fout:
    writer = csv.DictWriter(fout, fieldnames=fieldnames_f, delimiter=';')
    writer.writeheader()
    writer.writerows(filosofi_rows)