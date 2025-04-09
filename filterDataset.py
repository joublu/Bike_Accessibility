import os
import csv

base_dir = "Bike_Accessibility/Data_BA/"
dataset = "100N_1_reindexed"
new_dataset = "19N_1_reindexed"
in_folder = base_dir
out_folder = base_dir
# os.makedirs(out_folder, exist_ok=True)

# list of nodes to keep (unique)
L = {10, 14, 15, 17, 18, 19, 22, 23, 24, 25, 34, 68, 70, 71, 74, 82, 83, 84, 85}

def filter_noeuds():
    in_file = os.path.join(in_folder, dataset + "_noeuds.csv")
    out_file = os.path.join(out_folder, new_dataset + "_noeuds.csv")
    with open(in_file, newline='', encoding='utf-8') as fin, open(out_file, 'w', newline='', encoding='utf-8') as fout:
        reader = csv.reader(fin, delimiter=';')
        writer = csv.writer(fout, delimiter=';')
        header = next(reader)
        writer.writerow(header)
        for row in reader:
            try:
                node_id = int(row[0])
            except ValueError:
                continue
            if node_id in L:
                writer.writerow(row)

def filter_arcs():
    in_file = os.path.join(in_folder, dataset + "_arcs.csv")
    out_file = os.path.join(out_folder, new_dataset + "_arcs.csv")
    with open(in_file, newline='', encoding='utf-8') as fin, open(out_file, 'w', newline='', encoding='utf-8') as fout:
        reader = csv.reader(fin, delimiter=';')
        writer = csv.writer(fout, delimiter=';')
        header = next(reader)
        writer.writerow(header)
        start_idx = header.index("start_node")
        end_idx = header.index("end_node")
        for row in reader:
            try:
                start_node = int(row[start_idx-1])
                end_node = int(row[end_idx-1])
                # print(start_node, "-", end_node)
            except ValueError:
                continue
            if start_node in L and end_node in L:
                writer.writerow(row)

def filter_poi():
    in_file = os.path.join(in_folder, dataset + "_poi.csv")
    out_file = os.path.join(out_folder, new_dataset + "_poi.csv")
    with open(in_file, newline='', encoding='utf-8') as fin, open(out_file, 'w', newline='', encoding='utf-8') as fout:
        reader = csv.DictReader(fin, delimiter=';')
        fieldnames = reader.fieldnames
        writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=';')
        writer.writeheader()
        for row in reader:
            try:
                id_node = int(row["id_node"])
            except (ValueError, KeyError):
                continue
            if id_node in L:
                writer.writerow(row)

def filter_filosofi():
    in_file = os.path.join(in_folder, dataset + "_filosofi.csv")
    out_file = os.path.join(out_folder, new_dataset + "_filosofi.csv")
    with open(in_file, newline='', encoding='utf-8') as fin, open(out_file, 'w', newline='', encoding='utf-8') as fout:
        reader = csv.DictReader(fin, delimiter=';')
        fieldnames = reader.fieldnames
        writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=';')
        writer.writeheader()
        for row in reader:
            try:
                noeud_del = int(row["noeud_delegue"])
            except (ValueError, KeyError):
                continue
            if noeud_del in L:
                writer.writerow(row)

filter_noeuds()
filter_arcs()
filter_poi()
filter_filosofi()