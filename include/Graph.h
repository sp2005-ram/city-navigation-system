//Sri Rama Jayam

#pragma once
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<string>
#include<stdexcept>

//Data Types --------------------------------

struct Coordinates{
	double lat, lon;
};

struct Edge{
	int to; // neighbor
	double weight; // road Distance km
	double speed; // Speed in km/h
};

struct Node{
	int id;
	std::string name;
	Coordinates coords;
	std::vector<Edge> neighbors;
};

//Graph -----------------------------------

class Graph{
public:
	// Returns internal index assigned to the node
	int addNode(const std::string& name, double lat, double lon);

	// Undirected by default; set_direct = true for one-way roads
	void addEdge(const std::string& from, const std::string& to, double distancekm, double speedKmh=50.0, bool set_direct=false);

	int indexOf(const std::string& name) const; 
	const Node& nodeAt(int idx) const;

	int size() const& { return static_cast<int>(nodes_.size()) ;}

	// Haversine distance - distance between two points on a circle
	double haversine(int u, int v) const;
			
private:
	std::vector<Node> nodes_;
	std::unordered_map<std::string, int>nametoIdx_;
};
