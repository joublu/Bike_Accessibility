#pragma once

#include "Tile.h"

class Tiles
{
private:
	long int nb_tiles;
	long int nb_poi;
	vector<Tile*> list_of_tiles;
	vector<POI*> list_of_poi;

	long int size_var_tab;
	long int couple_size_var_tab;

	long int nb_ppoi_tile_couple;
public:
	Tiles(long int _nb = 0) : nb_tiles(_nb) {
		size_var_tab = 0; couple_size_var_tab = 0; nb_ppoi_tile_couple = 0;
	};

	//getters
	vector<Tile*>& getListeOfTiles() { return list_of_tiles; };
	vector<POI*>& getListOfPoi() { return list_of_poi; };
	
	long int getNbTiles() { return nb_tiles; };
	long int getNbPoi() { return nb_poi; };
	long int getsizeVarTab() { return size_var_tab; };
	long int getsizeCoupleVarTab() { return couple_size_var_tab; };

	long int getNbPpoiTileCouple() {return this->nb_ppoi_tile_couple;	};
	

	//setters
	void addTile(Tile* tile) {
		list_of_tiles.push_back(tile);
	};

	void setNbTiles(int _nb) { nb_tiles = _nb; };
	void setNbPoi(int _nb) { nb_poi = _nb; };

	//initialize reachable POI
	void initialize_reachable_poi();
	void initialize_reachable_poi_v2();

	void display_carreaux_data();


};

