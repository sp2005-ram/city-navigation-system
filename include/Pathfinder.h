//Sri Rama Jayam

#pragma once
#include "Graph.h"
#include<vector>
#include<string>
#include<unordered_map>
#include<functional>

//Result-----------------------

struct PathResult{
	bool found = false;
	double totalCost = 0.0;
	std::vector<int> indices;
	std::vector<std::string> names;
};

enum class WeightMode{
	DISTANCE,
	TIME
};

//Pathfinder --------------------

class Pathfinder{
public:
	explicit Pathfinder(const Graph& graph);

	// Core A* Query - Cache results by (src, dst, mode) key
	PathResult query(const std::string& src, const std::string& dst, WeightMode mode = WeightMode::DISTANCE);

	// Invalidate Cache Entries touching a node (call after live weight updates)
	void invalidateCache(const std::string& nodeName);
	void clearCache();
	int cacheSize() const { return static_cast<int>(cache_.size()); }


private:
	PathResult astar(int src, int dst, WeightMode mode);

	//Edge Weight depending on the chosen mode
	double edgeWeight(const Edge& e, WeightMode mode) const;
	//Reconstruct Path from a predecessor map
	PathResult buildResult(const std::vector<int>&prev, int src, int dst, double cost) const;
	// Cache key: "srcIdx:dstIdx:mode"
	std::string cacheKey(int src, int dst, WeightMode mode) const;

	const Graph& g_;

	// HashMap : cache key -> PathResult
	// Primary HashMap Integration : O(1) repeated query lookup
	std::unordered_map<std::string, PathResult> cache_;
};
