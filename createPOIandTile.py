'''
Script to help create a new POI file and a new tile file, from the dataset 100N
TODO bouger tous les fichiers de Tours_anonymized_100N_2 dans data_BA
TODO plutot copier coller le fichier des noeuds de Tours_anonymized_100N_2 dans data_BA
'''

# import os
import csv
import matplotlib.pyplot as plt
import random

dataset = "100N_2"
fullpath = f"Bike_Accessibility/Data_BA/{dataset}_noeuds.csv"
x = []
y = []
labels = [] # for node labels
# pour les tiles
# to compute the centroids
lmin, lmax = float('inf'), float('-inf')
Lmin, Lmax = float('inf'), float('-inf')
n=3
m=3
tile_centroids = []
# pour les POI
nbPOI = 12
new_poi_file = 0

with open(fullpath, newline='') as f:
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

''' to put the correct nodes in the POI file 
(before that a POI file must be created by copy pasting the first nbPOI lines
from the tours POI file)'''
if new_poi_file:
	indices = random.sample(range(len(x)), nbPOI)
	print(indices)
	random_nodes = [(x[i], y[i], labels[i]) for i in indices]
	poi_file = f"Bike_Accessibility/Data_BA/{dataset}_poi.csv"
	with open(poi_file, newline='') as fin:
		reader = csv.DictReader(fin, delimiter=';')
		rows = list(reader)
		fieldnames = reader.fieldnames
	for i, row in enumerate(rows):
		row["id_node"] = random_nodes[i][2]
	with open(poi_file, 'w', newline='') as fout:
		writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=';')
		writer.writeheader()
		writer.writerows(rows)
	for poi in random_nodes:
		plt.plot(poi[0], poi[1], 'go', markersize=10, alpha=0.5)
		plt.annotate(poi[2], (poi[0], poi[1]))


''' Pour créer le fichier des tiles
TODO change the file like with the POI section'''
# print(f"l={lmax-lmin}, L={Lmax-Lmin}")
l = (lmax-lmin) / n
L = (Lmax-Lmin) / m

for i in range(n):
	for j in range(m):
		x_min = i * l + lmin
		x_max = (i + 1) * l + lmin
		y_min = j *L + Lmin
		y_max = (j + 1) * L+ Lmin
		# # plot the grid
		# plt.plot(x_min, y_min, 'go')
		# plt.plot(x_max, y_max, 'go')
		# plt.plot(x_min, y_max, 'go')
		# plt.plot(x_max, y_min, 'go')
		points_in_tile = [(xx, yy, lab) for xx, yy, lab in zip(x, y, labels) if x_min <= xx < x_max and y_min <= yy < y_max]
		if points_in_tile:
			avg_x = sum(pt[0] for pt in points_in_tile) / len(points_in_tile)
			avg_y = sum(pt[1] for pt in points_in_tile) / len(points_in_tile)
			dmin=float('inf')
			for k in range(len(points_in_tile)):
				d = ((points_in_tile[k][0]-avg_x)**2 + (points_in_tile[k][1]-avg_y)**2)**(1/2)
				if d < dmin:
					centroid = points_in_tile[k]
					dmin = d
			tile_centroids.append(centroid)
		else:
			tile_centroids.append(None) # to keep the ordrer

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

plt.title(f"Noeuds et centroides des carreaux ({n}*{m} carreaux), {dataset}")
plt.xlabel("X")
plt.ylabel("Y")
plt.show()