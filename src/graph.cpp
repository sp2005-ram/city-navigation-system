//Sri Rama Jayam

#include "Graph.h"
#include <cmath>
#include <stdexcept>

static constexpr double EARTH_RADIUS = 6371.0;
static constexpr double DEG2RAD = M_PI/180;

// Node Management -------------------------------

int Graph::addNode(const std::string& name, double lat, double lon){
	if (nametoIdx_.count(name)){
		return nametoIdx_[name];
	}
	int idx = static_cast<int>(nodes_.size());
	nodes_.push_back({idx, name, {lat, lon}, {}});
	nametoIdx_[name] = idx;
	return idx;
}

int Graph::indexOf(const std::string& name) const {
	auto it = nametoIdx_.find(name);
	if (it == nametoIdx_.end()){
		throw std::invalid_argument("Node not found: " + name);
	}
	return it->second;
}

const Node& Graph::nodeAt(int idx) const{
	return nodes_.at(idx);
}

// Edge Management ----------------------------
void Graph::addEdge(const std::string& from, const std::string& to, double distanceKm, double speedKmh, bool directed){
	int u = indexOf(from);
	int v = indexOf(to);
	nodes_[u].neighbors.push_back({v, distanceKm, speedKmh});
	if (! directed){
		nodes_[v].neighbors.push_back({u, distanceKm, speedKmh});
	}	
}

// Haversine Heuristic--------------------------

double Graph::haversine(int u, int v) const{
	const auto& a = nodes_[u].coords;
	const auto& b = nodes_[v].coords;
	double dLat = (b.lat - a.lat) * DEG2RAD;
	double dLon = (b.lon - a.lon) * DEG2RAD;
	double aLat = a.lat * DEG2RAD;
	double bLat = b.lat * DEG2RAD;
	double h = std::sin(dLat/2)*std::sin(dLat/2) + std::cos(aLat)*std::cos(bLat)*std::sin(dLon/2)*std::sin(dLon/2);
	return 2*EARTH_RADIUS*std::asin(std::sqrt(h));
}
