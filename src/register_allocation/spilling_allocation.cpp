#include "spilling_allocation.h"
#include "basic_allocation.h"
#include "../graph_builder/graph_builder.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

// Returns the web ID with the highest degree among the internally-spilled webs.
static int pickWebToSpill(const vector<Web>& active, const Graph<int>& g) {
    int bestId  = -1;
    int bestDeg = -1;

    for (const auto& w : active) {
        if (w.reg != -1) continue;                    // already coloured
        Vertex<int>* v = g.findVertex(w.id);
        int deg = v ? (int)v->getAdj().size() : 0;
        if (deg > bestDeg) { bestDeg = deg; bestId = w.id; }
    }
    return bestId;
}

AllocationResult spillingAllocation(vector<Web>& webs, int numRegisters, int maxSpills) {

    set<int> forcedSpills;

    for (int attempt = 0; attempt <= maxSpills; attempt++) {

        // Build the active subset (not explicitly spilled)
        vector<Web> active;
        for (const auto& w : webs)
            if (!forcedSpills.count(w.id))
                active.push_back(w);

        Graph<int> g  = buildInterferenceGraph(active);
        AllocationResult tmp = basicAllocation(g, active, numRegisters);

        bool clean = all_of(active.begin(), active.end(),
            [](const Web& w) { return w.reg >= 0; });

        if (clean) {
            // Copy results back and build full AllocationResult
            unordered_map<int,int> regMap;
            for (const auto& aw : active)
                regMap[aw.id] = aw.reg;

            AllocationResult full;
            full.usedRegisters = tmp.usedRegisters;

            for (auto& w : webs) {
                if (forcedSpills.count(w.id)) {
                    w.reg = -1;
                } else {
                    w.reg = regMap[w.id];
                    if (w.reg >= 0) full.webToRegister[w.id] = w.reg;
                }
            }
            return full;
        }

        if (attempt == maxSpills) break;

        // Choose the highest-degree internally-spilled web to force-spill next
        int spillId = pickWebToSpill(active, g);
        if (spillId != -1) forcedSpills.insert(spillId);
    }

    cout << "\nWarning: spilling allocation failed with " << numRegisters
         << " register(s) and at most " << maxSpills << " spill(s). All webs assigned to memory.\n";
    AllocationResult fail;
    fail.usedRegisters = 0;
    for (auto& w : webs) w.reg = -1;
    return fail;
}
