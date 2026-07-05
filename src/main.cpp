#include "Graph.h"
#include "Pathfinder.h"
#include <iostream>
#include <iomanip>
#include <chrono>

// ─── Helpers ──────────────────────────────────────────────────────────────────

void printResult(const PathResult& r, WeightMode mode) {
    if (!r.found) {
        std::cout << "  [No path found]\n";
        return;
    }
    std::cout << "  Path:  ";
    for (size_t i = 0; i < r.names.size(); ++i) {
        if (i) std::cout << " → ";
        std::cout << r.names[i];
    }
    std::cout << "\n";
    if (mode == WeightMode::DISTANCE)
        std::cout << "  Cost:  " << std::fixed << std::setprecision(2)
                  << r.totalCost << " km\n";
    else
        std::cout << "  Cost:  " << std::fixed << std::setprecision(1)
                  << r.totalCost * 60.0 << " min\n";
}

void benchmark(Pathfinder& pf, const std::string& src, const std::string& dst,
               WeightMode mode, const std::string& label) {
    std::cout << "\n── " << label << " ──\n";
    std::cout << "  " << src << " → " << dst << "\n";

    auto t0 = std::chrono::high_resolution_clock::now();
    auto r1  = pf.query(src, dst, mode);
    auto t1  = std::chrono::high_resolution_clock::now();
    printResult(r1, mode);
    std::cout << "  Time (cold):   "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()
              << " µs\n";

    auto t2 = std::chrono::high_resolution_clock::now();
    pf.query(src, dst, mode);
    auto t3  = std::chrono::high_resolution_clock::now();
    std::cout << "  Time (cached): "
              << std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()
              << " µs  (cache entries: " << pf.cacheSize() << ")\n";
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    Graph g;

    // ── NYC Landmarks (lat, lon) ───────────────────────────────────────────
    // Lower Manhattan
    g.addNode("Battery Park",         40.7033, -74.0170);
    g.addNode("Wall Street",          40.7074, -74.0113);
    g.addNode("Fulton Center",        40.7098, -74.0072);
    g.addNode("City Hall",            40.7128, -74.0059);
    g.addNode("Canal St",             40.7193, -74.0021);

    // Midtown
    g.addNode("Union Square",         40.7359, -73.9906);
    g.addNode("Flatiron",             40.7411, -73.9897);
    g.addNode("Penn Station",         40.7506, -73.9971);
    g.addNode("Times Square",         40.7580, -73.9855);
    g.addNode("Grand Central",        40.7527, -73.9772);
    g.addNode("Rockefeller Center",   40.7587, -73.9787);
    g.addNode("Columbus Circle",      40.7681, -73.9819);

    // Upper Manhattan
    g.addNode("Lincoln Center",       40.7725, -73.9835);
    g.addNode("American Museum",      40.7813, -73.9740);   // AMNH
    g.addNode("Central Park North",   40.7998, -73.9528);
    g.addNode("Harlem - 125th St",    40.8079, -73.9456);
    g.addNode("Washington Heights",   40.8448, -73.9393);

    // Brooklyn
    g.addNode("Brooklyn Bridge",      40.7061, -73.9969);
    g.addNode("DUMBO",                40.7033, -73.9881);
    g.addNode("Atlantic Terminal",    40.6845, -73.9770);
    g.addNode("Prospect Park",        40.6602, -73.9690);
    g.addNode("Coney Island",         40.5755, -73.9707);

    // Queens
    g.addNode("Long Island City",     40.7447, -73.9485);
    g.addNode("Jackson Heights",      40.7556, -73.8830);
    g.addNode("Flushing",             40.7676, -73.8330);
    g.addNode("JFK Airport",          40.6413, -73.7781);

    // Bronx
    g.addNode("Yankee Stadium",       40.8296, -73.9262);
    g.addNode("Fordham",              40.8619, -73.8993);

    // ── Edges: addEdge(from, to, distKm, speedKmh, directed) ──────────────
    // Lower Manhattan spine (Broadway / Broadway-adjacent)
    g.addEdge("Battery Park",       "Wall Street",        0.7,  20.0);
    g.addEdge("Wall Street",        "Fulton Center",      0.5,  20.0);
    g.addEdge("Fulton Center",      "City Hall",          0.5,  20.0);
    g.addEdge("City Hall",          "Canal St",           0.8,  25.0);
    g.addEdge("Canal St",           "Union Square",       2.6,  30.0);

    // Midtown grid
    g.addEdge("Union Square",       "Flatiron",           0.6,  25.0);
    g.addEdge("Flatiron",           "Penn Station",       1.2,  25.0);
    g.addEdge("Penn Station",       "Times Square",       0.9,  20.0);   // always congested
    g.addEdge("Times Square",       "Rockefeller Center", 0.6,  20.0);
    g.addEdge("Grand Central",      "Rockefeller Center", 0.6,  25.0);
    g.addEdge("Grand Central",      "Times Square",       0.8,  20.0);
    g.addEdge("Flatiron",           "Grand Central",      1.4,  30.0);
    g.addEdge("Rockefeller Center", "Columbus Circle",    1.2,  30.0);

    // Upper Manhattan (Riverside / Broadway corridor)
    g.addEdge("Columbus Circle",    "Lincoln Center",     0.7,  30.0);
    g.addEdge("Lincoln Center",     "American Museum",    1.2,  30.0);
    g.addEdge("American Museum",    "Central Park North", 2.1,  35.0);
    g.addEdge("Central Park North", "Harlem - 125th St",  1.0,  30.0);
    g.addEdge("Harlem - 125th St",  "Washington Heights", 4.1,  40.0);
    g.addEdge("Washington Heights", "Yankee Stadium",     4.5,  45.0);
    g.addEdge("Yankee Stadium",     "Fordham",            3.8,  50.0);

    // East side cross-links (Lex / 2nd Ave)
    g.addEdge("Union Square",       "Grand Central",      1.5,  28.0);
    g.addEdge("Grand Central",      "Lincoln Center",     2.8,  35.0);   // via 42nd/57th
    g.addEdge("Harlem - 125th St",  "Yankee Stadium",     3.2,  45.0);   // Madison Bridge

    // Brooklyn connections
    g.addEdge("City Hall",          "Brooklyn Bridge",    0.6,  25.0);
    g.addEdge("Brooklyn Bridge",    "DUMBO",              0.9,  20.0);
    g.addEdge("DUMBO",              "Atlantic Terminal",  2.8,  35.0);
    g.addEdge("Atlantic Terminal",  "Prospect Park",      2.5,  35.0);
    g.addEdge("Prospect Park",      "Coney Island",      10.0,  50.0);   // Ocean Pkwy
    g.addEdge("Wall Street",        "Brooklyn Bridge",    0.9,  20.0);   // via Beekman

    // Queens connections
    g.addEdge("Grand Central",      "Long Island City",   3.5,  40.0);   // Queens-Midtown Tunnel
    g.addEdge("Long Island City",   "Jackson Heights",    4.8,  45.0);
    g.addEdge("Jackson Heights",    "Flushing",           7.2,  50.0);
    g.addEdge("Flushing",           "JFK Airport",       15.0,  65.0);   // Van Wyck Expy
    g.addEdge("Atlantic Terminal",  "JFK Airport",       16.5,  60.0);   // Belt Pkwy
    g.addEdge("Long Island City",   "JFK Airport",       20.0,  60.0);
    g.addEdge("DUMBO",              "Long Island City",   3.2,  40.0);   // Manhattan Bridge / LIC

    // ── Queries ───────────────────────────────────────────────────────────
    Pathfinder pf(g);

    std::cout << "=== NYC City Navigation System ===\n";
    std::cout << "Graph: " << g.size() << " nodes, A* with Haversine heuristic\n";

    benchmark(pf, "Battery Park",    "JFK Airport",     WeightMode::DISTANCE, "Battery Park → JFK  [Shortest km]");
    benchmark(pf, "Battery Park",    "JFK Airport",     WeightMode::TIME,     "Battery Park → JFK  [Fastest time]");
    benchmark(pf, "Coney Island",    "Washington Heights", WeightMode::DISTANCE, "Coney Island → Washington Heights [Distance]");
    benchmark(pf, "Coney Island",    "Washington Heights", WeightMode::TIME,     "Coney Island → Washington Heights [Time]");
    benchmark(pf, "Times Square",    "Fordham",         WeightMode::TIME,     "Times Square → Fordham [Time]");
    benchmark(pf, "Penn Station",    "Flushing",        WeightMode::DISTANCE, "Penn Station → Flushing [Distance]");

    // ── Cache invalidation: simulate a traffic incident at Times Square ───
    std::cout << "\n── Simulating traffic incident at Times Square ──\n";
    std::cout << "  Cache entries before: " << pf.cacheSize() << "\n";
    pf.invalidateCache("Times Square");
    std::cout << "  Cache entries after:  " << pf.cacheSize()
              << "  (routes through Times Square evicted)\n";

    // Re-query after invalidation — forces fresh A* run
    std::cout << "\n  Re-routing Penn Station → Flushing after incident:\n  ";
    auto fresh = pf.query("Penn Station", "Flushing", WeightMode::TIME);
    printResult(fresh, WeightMode::TIME);

    return 0;
}
