//Sri Rama Jayam

#include "Pathfinder.h"
#include<queue>
#include<vector>
#include<limits>
#include<sstream>
#include<algorithm>

static constexpr double INF = std::numeric_limits<double>::infinity();

//-------PQ Entry : (f_score, node_index)

using PQEntry = std::pair<double,int>;
struct MinHeap{
	bool operator()(PQEntry& a, PQEntry& b) const{
		return a.first > b.first; // min heap on f
	}
};

// Constructor -----------
Pathfinder::Pathfinder(const Graph& graph) : g_(graph) {};

// Public-Query (cache-aware) ---------

PathResult Pathfinder::query(const std::string& src, const std::string& dst, WeightMode mode){
	int s = g_.indexOf(src);
	int d = g_.indexOf(dst);

	std::string key = cacheKey(s, d, mode);
	auto it = cache_.find(key);
	if (it != cache_.end()){
		return it->second;
	}
	PathResult result = astar(s,d,mode);
	cache_[key] = result;
	return result;
}

// Core A* -----------------------
PathResult Pathfinder::astar(int src, int dst, WeightMode mode){
	const int N = g_.size();

	std::vector<double> g(N, INF); // actual cost from source
	std::vector<double> f(N, INF); // g + heuristic
	std::vector<int> prev(N, -1);
	std::vector<bool> closed(N, false);

	g[src] = 0.0;
	f[src] = g_.haversine(src, dst); // heuristic in km; admissible for DISTANCE mode
                                         // slightly inadmissible for TIME — acceptable tradeoff
                                         // (use h=0 if strict optimality needed for TIME)
	std::priority_queue<PQEntry, std::vector<PQEntry>, MinHeap>pq;
	pq.push({ f[src], src });
	while (! pq.empty()){
		auto [fVal, u] = pq.top(); pq.pop();

		// Lazy Detection -  skip stale entries
		if (closed[u]) continue;
		closed[u] = true;
		
		// Goal Reached 
		if (u == dst){
			return buildResult(prev, src, dst, g[dst]);
		}

		for (const Edge& e: g_.nodeAt(u).neighbors){
			int v = e.to;
			if (closed[v]) continue;

			double tentative_g = g[u] + edgeWeight(e, mode);
			if (tentative_g < g[v]){
				g[v] = tentative_g;
				prev[v] = u;
				// Heuristic: straight-line km distance to goal
				// For TIME mode this is a slight underestimate (optimistic) — still admissible
				f[v] = g[v] + g_.haversine(v,dst);
				pq.push({ f[v], v});
			}
		}
	}
	return PathResult{};
}

// Edge Weight -----------------------

double Pathfinder::edgeWeight(const Edge& e, WeightMode mode) const{
	if (mode == WeightMode::DISTANCE){
		return e.weight;
	}
	return e.weight/e.speed;
}

// Path Reconstruction ---------------------------------

PathResult Pathfinder::buildResult(const std::vector<int>& prev, int src, int dst, double cost) const {
	PathResult res;
	res.found = true;
	res.totalCost = cost;

	// Walk predecessor chain backward
	for (int cur = dst; cur != -1; cur = prev[cur]){
		res.indices.push_back(cur);
	}

	std::reverse(res.indices.begin(), res.indices.end());

	for (int idx : res.indices){
		res.names.push_back(g_.nodeAt(idx).name);
	}
	return res;
}

// Cache Management --------------------

std::string Pathfinder::cacheKey(int src, int dst, WeightMode mode) const{
	return std::to_string(src) + ":" + std::to_string(dst) + ":" + std::to_string(static_cast<int>(mode));
}

void Pathfinder::invalidateCache(const std::string& nodeName){
	int idx = g_.indexOf(nodeName);
	std::string s = std::to_string(idx) + ":";
	for (auto it = cache_.begin(); it != cache_.end(); ){
		if (it->first.rfind(s,0) == 0 || it->first.find(":"+std::to_string(idx)+":") != std::string::npos){
			it = cache_.erase(it);
		}
		else{
			++it;
		}
	}
}

void Pathfinder::clearCache() {
	cache_.clear();
}
