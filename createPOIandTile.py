'''
Script to create a new POI file and a new tile file

HOW TO USE:
1 Mettre le bon dataset
2 si dataset de felix, dans le fichier nodes changer les ; en , et supp les ( et )
3 pour avoir la meme densité que tours :
    - les POI doivent etre 1 POI pour 10 noeuds
    - le nombre de tiles doit etre 5 noeuds par tile
'''

# import os
import csv
import matplotlib.pyplot as plt
from random import sample as random_sample
from shutil import copy as shutil_copy

dataname = "1000N_0"
dataset = f"export_instance/Tours_anonymized_1000N/Tours_anonymized_{dataname}"
nodes_file = f"Bike_Accessibility/{dataset}/clean_data_nodes.csv"
arcs_file = f"Bike_Accessibility/{dataset}/clean_data_arcs.csv"

## copy nodes and arcs files
output_nodes_file = f"Bike_Accessibility/Data_BA/{dataname}_noeuds.csv"
output_arcs_file  = f"Bike_Accessibility/Data_BA/{dataname}_arcs.csv"
shutil_copy(nodes_file, output_nodes_file)
shutil_copy(arcs_file, output_arcs_file)

source_poi_file = "Bike_Accessibility/Data_BA/Tours_poi.csv"
source_filosofi_file = "Bike_Accessibility/Data_BA/Tours_filosofi.csv"

output_poi_file = f"Bike_Accessibility/Data_BA/{dataname}_poi.csv"
output_filosofi_file = f"Bike_Accessibility/Data_BA/{dataname}_filosofi.csv"

print("nbPOI = number of POI to create, (n,m) = n*m tiles")
nbPOI=int(input("nbPOI"))
n=int(input("n"))
m=int(input("m"))
input("/!\ LANCER CE SCRIPT CHANGERA LES FICHIERS POI")

# nbPOI = 12
# n = 3
# m = 3

x = []
y = []
labels = [] # for node labels
thisdict= {}

lmin, lmax = float('inf'), float('-inf')
Lmin, Lmax = float('inf'), float('-inf')

with open(nodes_file, newline='') as f:
	next(f) # skip header
	reader = csv.reader(f, delimiter=';')
	for row in reader:
		# coord = row[1][1:-1].split(",")
		# x.append(float(coord[0]))
		# y.append(float(coord[1]))
		x.append(float(row[1]))
		y.append(float(row[2]))
		coord=[float(row[1]), float(row[2])]
		if float(coord[0])<lmin:
			lmin=float(coord[0])
		if float(coord[0])>lmax:
			lmax=float(coord[0])
		if float(coord[1])<Lmin:
			Lmin=float(coord[1])
		if float(coord[1])>Lmax:
			Lmax=float(coord[1])
		# # for better visibility, only display a few tags
		# if random.random() < 0.75:
		#     tag=""
		# else:
		#     tag=row[0]
		# n.append(tag)
		label=row[0]
		labels.append(label)
		# print(label)
		thisdict.update({int(label): coord})

## POI
poi_rows = []
with open(source_poi_file, newline='') as fin:
    reader = csv.DictReader(fin, delimiter=';')
    fieldnames = reader.fieldnames
    for i, row in enumerate(reader):
        if i >= nbPOI:
            break
        poi_rows.append(row)
indices =random_sample(range(len(x)), nbPOI)
random_nodes = [(x[i], y[i], labels[i]) for i in indices]
for i, row in enumerate(poi_rows):
    row["id_node"] = random_nodes[i][2]

# Write new POI file
with open(output_poi_file, 'w', newline='') as fout:
    writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=';')
    writer.writeheader()
    writer.writerows(poi_rows)

# # things to plot
# for poi in random_nodes:
#     plt.plot(poi[0], poi[1], 'go', markersize=10, alpha=0.5)
#     plt.annotate(poi[2], (poi[0], poi[1]))


## filosofi
tile_centroids = []
tile_width = (lmax - lmin) / n
tile_height = (Lmax - Lmin) / m

for i in range(n):
    for j in range(m):
        x_min = i * tile_width + lmin
        x_max = (i + 1) * tile_width + lmin
        y_min = j * tile_height + Lmin
        y_max = (j + 1) * tile_height + Lmin
        # # plot the grid
        plt.plot(x_min, y_min, 'go')
        plt.plot(x_max, y_max, 'go')
        plt.plot(x_min, y_max, 'go')
        plt.plot(x_max, y_min, 'go')
        points_in_tile = [(xx, yy, lab) for xx, yy, lab in zip(x, y, labels) if x_min <= xx < x_max and y_min <= yy < y_max]
        if points_in_tile:
            avg_x = sum(pt[0] for pt in points_in_tile) / len(points_in_tile)
            avg_y = sum(pt[1] for pt in points_in_tile) / len(points_in_tile)
            dmin=float('inf')
            centroid = None
            for pt in points_in_tile:
                d = ((pt[0]-avg_x)**2 + (pt[1]-avg_y)**2)**0.5
                if d < dmin:
                    dmin = d
                    centroid = pt
            tile_centroids.append(centroid)
        else:
            tile_centroids.append(None) # preserve order

plt.scatter(x, y)

for centroid in tile_centroids:
	if centroid is not None:
		plt.plot(centroid[0], centroid[1], 'ro', markersize=10, alpha=0.5)
		plt.annotate(centroid[2], (centroid[0], centroid[1]))

# print(lmin, lmax, Lmin, Lmax)
# plt.plot(lmin, Lmin, 'ro')
# plt.plot(lmin, Lmax, 'ro')
# plt.plot(lmax, Lmin, 'ro')
# plt.plot(lmax, Lmax, 'ro')

# for i, txt in enumerate(n):
#     plt.annotate(txt, (x[i], y[i]))

centroid_labels = [centroid[2] for centroid in tile_centroids if centroid is not None]
# print("Centroid labels:", centroid_labels)

# Plot arcs
with open(arcs_file, newline='') as f:
    reader = csv.DictReader(f, delimiter=';')
    for row in reader:
        startn = int(row["start_node"])
        endn = int(row["end_node"])
        # startn = int(row["nodeI"])
        # endn = int(row["nodeJ"])
        x_start, y_start = thisdict[startn][0], thisdict[startn][1]
        x_end, y_end = thisdict[endn][0], thisdict[endn][1]
        dx = x_end - x_start
        dy = y_end - y_start
        plt.arrow(x_start, y_start, dx, dy, width=0.00001, alpha=0.5)

plt.title(f"Noeuds et centroides des carreaux ({n}*{m} carreaux), {dataset}")
plt.xlabel("X")
plt.ylabel("Y")
plt.show()

filosofi_rows = []
with open(source_filosofi_file, newline='') as fin:
    reader = csv.DictReader(fin, delimiter=';')
    fieldnames_f = reader.fieldnames
    for i, row in enumerate(reader):
        if i >= n*m:
            break
        filosofi_rows.append(row)

new_filosofi_rows = []
for idx, row in enumerate(filosofi_rows):
    if idx < len(tile_centroids) and tile_centroids[idx] is not None:
        row["noeud_delegue"] = tile_centroids[idx][2]
        new_filosofi_rows.append(row)
    # otherwise, skip the row

with open(output_filosofi_file, 'w', newline='') as fout:
    writer = csv.DictWriter(fout, fieldnames=fieldnames_f, delimiter=';')
    writer.writeheader()
    writer.writerows(new_filosofi_rows)