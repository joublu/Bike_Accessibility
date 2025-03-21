#pragma once
class POI
{
private :
	long int id_poi;
	long int id_poi_node;
public:
	POI(long int _id_poi, long int _id_node) : id_poi(_id_poi), id_poi_node(_id_node) {};

	//getters
	long int getPoiId() { return id_poi; };
	long int getPoiNode() { return id_poi_node; };

	//setters
};

