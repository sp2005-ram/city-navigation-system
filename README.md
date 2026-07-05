# City Navigation System — NYC

A real-time pathfinding engine built in C++ using a customized A* algorithm, modeled on the NYC road network. Supports dual optimization modes (shortest distance / fastest time), HashMap-backed route caching, and live traffic invalidation.

---

## Features

- 'A\*' **Pathfinding** with a Haversine great-circle distance heuristic — admissible by construction since road distances always exceed straight-line surface distance
- **Dual weight modes** — switch between minimizing kilometers or minimizing travel time at query time, without changing the underlying graph
- **HashMap route cache** — `unordered_map` keyed on `(src, dst, mode)` triples; repeat queries resolve in <1µs vs ~15µs for a cold A* run
- **Targeted cache invalidation** — evicts only routes touching an updated node, leaving unaffected cached routes intact
- **Haversine heuristic** — computes great-circle distance between two GPS coordinates as the A* lower-bound estimate; always admissible for road networks
- **Incremental Makefile** — `-MMD -MP` dependency tracking ensures only translation units affected by a header change are recompiled

---

## Project Structure

```
city-navigation/
├── include/
│   ├── Graph.h          # Node, Edge, Coordinates structs; adjacency-list graph
│   └── Pathfinder.h     # A* engine, WeightMode enum, PathResult struct
├── src/
│   ├── graph.cpp        # Graph construction, Haversine implementation
│   ├── pathfinder.cpp   # A* search, route cache, cache invalidation
│   └── main.cpp         # NYC graph definition, benchmark queries
├── Makefile
└── README.md
```

---

## Build & Run

**Requirements:** g++ with C++17 support

```bash
make        # compile
make run    # compile + run
make clean  # remove build artifacts
```

---

## Sample Output

```
=== NYC City Navigation System ===
Graph: 28 nodes, A* with Haversine heuristic

── Battery Park → JFK [Shortest km] ──
  Path:  Battery Park → Wall Street → Brooklyn Bridge → DUMBO → Atlantic Terminal → JFK Airport
  Cost:  21.80 km
  Time (cold):    15 µs
  Time (cached):   1 µs

── Coney Island → Washington Heights [Distance vs Time] ──
  Distance path: ... → Grand Central → Rockefeller Center → Columbus Circle → Lincoln Center → ...
  Time path:     ... → Grand Central → Lincoln Center → ...   (skips Rockefeller; faster edge)

── Simulating traffic incident at Times Square ──
  Cache entries before: 6
  Cache entries after:  5  (routes through Times Square evicted)
```

Distance and time modes produce different routes — A* correctly optimizes for two independent cost functions on the same graph.

---

## Core Algorithm

Standard A* with `f(n) = g(n) + h(n)`, where:

| Term | Meaning |
|------|---------|
| `g(n)` | Actual cost from source to node `n` (km or hours) |
| `h(n)` | Haversine distance from `n` to goal (admissible lower bound) |
| `f(n)` | Estimated total path cost through `n` |

Lazy deletion handles stale priority queue entries — no decrease-key required. Path reconstruction via predecessor array, reversed after goal is reached.

---

## Data Structures

| Structure | Role |
|-----------|------|
| `vector<Node>` | Adjacency-list graph; O(1) node access by index |
| `unordered_map<string, int>` | Name → index mapping; O(1) node lookup by name |
| `priority_queue` (min-heap) | A* frontier ordered by `f(n)` |
| `unordered_map<string, PathResult>` | Route cache; O(1) repeat query resolution |

---

## Roadmap

- [ ] Time-dependent edge weights (rush hour multipliers)
- [ ] Bidirectional A* (meet-in-the-middle, ~2× fewer nodes explored)
- [ ] OpenStreetMap `.osm` file parser for real NYC road data
- [ ] Contraction Hierarchies for sub-millisecond queries on large graphs
