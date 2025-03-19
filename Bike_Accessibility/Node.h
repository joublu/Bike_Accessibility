#pragma once

#include <limits>
#include "POI.h"
#include <vector>

using namespace std;

class Node
{
private:
	long int id, name;
	double coord_x, coord_y;
	bool isVisisted; // for dijsktra
	double dist_label; // for dijsktra
	vector<POI*> list_of_attached_poi;
public:
	Node(long int _id, long int _name, double _x, double _y) : id(_id), name(_name), coord_x(_x), coord_y(_y), isVisisted(false), dist_label(std::numeric_limits<double>::infinity()) {};

	//getters
	long int getId() { return id; };
	long int getName() {return name; };
	double getCoordX() { return coord_x; };
	double getCoordY() { return coord_y; };
	bool getIsVisited() { return isVisisted; };
	double getDist() const { return dist_label; };
	vector<POI*>& getListOfAttachedPoi() { return list_of_attached_poi; };

	//setters
	void setIsVisisted(bool _b) { isVisisted = _b; };
	void setDistanceToInfinity() { dist_label = std::numeric_limits<double>::infinity() ; 	};
	void setDistance(double _d) { dist_label = _d; };

	void addPoi(POI* _p)	{	list_of_attached_poi.push_back(_p);	};
};

