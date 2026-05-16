#include "dsatur_allocation.h"

#include <set>
#include <unordered_map>

using namespace std;

// Tarjan DFS to find articulation points.
static void articulationDFS(
    int id,
    int parentId,
    int& timer,
    unordered_map<int, int>& disc,
    unordered_map<int, int>& low,
    unordered_map<int, bool>& isArt,
    const Graph<int>& graph
) {
    disc[id] = low[id] = timer++;
    int children = 0;

    Vertex<int>* v = graph.findVertex(id);
    for (auto* edge : v->getAdj()) {
        int nid = edge->getDest()->getInfo();
        if (disc[nid] == -1) {
            children++;
            articulationDFS(nid, id, timer, disc, low, isArt, graph);
            low[id] = min(low[id], low[nid]);
            if (parentId == -1 && children > 1) isArt[id] = true;
            if (parentId != -1 && low[nid] >= disc[id]) isArt[id] = true;
        } else if (nid != parentId) {
            low[id] = min(low[id], disc[nid]);
        }
    }
}

static unordered_map<int, bool> findArticulationPoints(const Graph<int>& graph) {
    unordered_map<int, int>  disc, low;
    unordered_map<int, bool> isArt;
    for (auto* v : graph.getVertexSet()) {
        disc[v->getInfo()] = -1;
        low[v->getInfo()]  =  0;
        isArt[v->getInfo()] = false;
    }
    int timer = 0;
    for (auto* v : graph.getVertexSet())
        if (disc[v->getInfo()] == -1)
            articulationDFS(v->getInfo(), -1, timer, disc, low, isArt, graph);
    return isArt;
}

AllocationResult dsaturAllocation(
    const Graph<int>& graph,
    vector<Web>& webs,
    int numRegisters
) {
    const int N = numRegisters;
    const auto& vertexSet = graph.getVertexSet();

    // colour[id]: -1 = uncoloured, -2 = spilled, >= 0 = assigned colour
    unordered_map<int, int> colour;
    for (auto* v : vertexSet)
        colour[v->getInfo()] = -1;

    // saturation[id]: set of distinct colours used by already-coloured neighbours
    unordered_map<int, set<int>> sat;
    for (auto* v : vertexSet)
        sat[v->getInfo()] = {};

    // Articulation points are structurally critical: colour them earlier so
    // they have the most colour choices available (additional heuristic).
    unordered_map<int, bool> isArt = findArticulationPoints(graph);

    set<int> usedColours;

    for (int step = 0; step < (int)vertexSet.size(); step++) {

        // Pick the uncoloured vertex with the highest saturation; break ties
        // by degree, then prefer articulation points.
        Vertex<int>* best    = nullptr;
        int          bestSat = -1;
        int          bestDeg = -1;
        bool         bestArt = false;

        for (auto* v : vertexSet) {
            int id = v->getInfo();
            if (colour[id] != -1) continue;

            int  s   = (int)sat[id].size();
            int  d   = (int)v->getAdj().size();
            bool art = isArt[id];

            if (s > bestSat
                || (s == bestSat && d > bestDeg)
                || (s == bestSat && d == bestDeg && art && !bestArt)) {
                bestSat = s;
                bestDeg = d;
                bestArt = art;
                best    = v;
            }
        }

        if (!best) break;

        int id = best->getInfo();

        // Assign the lowest colour not used by any neighbour
        int assigned = -1;
        for (int c = 0; c < N; c++) {
            if (!sat[id].count(c)) { assigned = c; break; }
        }

        if (assigned == -1) {
            colour[id] = -2;   // spill: all N colours are taken by neighbours
        } else {
            colour[id] = assigned;
            usedColours.insert(assigned);

            // Propagate the new colour to the saturation sets of uncoloured neighbours
            for (auto* edge : best->getAdj()) {
                int nid = edge->getDest()->getInfo();
                if (colour[nid] == -1)
                    sat[nid].insert(assigned);
            }
        }
    }

    AllocationResult result;
    result.usedRegisters = (int)usedColours.size();

    for (auto& web : webs) {
        int c = colour[web.id];
        if (c >= 0) {
            web.reg = c;
            result.webToRegister[web.id] = c;
        } else {
            web.reg = -1;
        }
    }

    return result;
}
