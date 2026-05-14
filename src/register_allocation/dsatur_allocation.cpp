#include "dsatur_allocation.h"

#include <set>
#include <unordered_map>

using namespace std;

// DSATUR (Degree of SATURation) graph colouring algorithm.
//
// At each step the uncoloured vertex with the highest saturation (number of
// distinct colours already assigned to its neighbours) is selected next.
// Ties are broken by choosing the vertex with the highest degree in the
// original graph.  The lowest available colour < numRegisters is then
// assigned; if none exists the web is spilled to memory.
//
// Compared to the basic greedy algorithm, DSATUR adapts its vertex ordering
// dynamically, which typically produces colourings closer to the chromatic
// number without exhaustive search.  Spilled webs are reported individually
// (partial allocation), rather than failing all at once.

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

    set<int> usedColours;

    for (int step = 0; step < (int)vertexSet.size(); step++) {

        // Pick the uncoloured vertex with the highest saturation; break ties
        // by original graph degree.
        Vertex<int>* best    = nullptr;
        int          bestSat = -1;
        int          bestDeg = -1;

        for (auto* v : vertexSet) {
            int id = v->getInfo();
            if (colour[id] != -1) continue;

            int s = (int)sat[id].size();
            int d = (int)v->getAdj().size();

            if (s > bestSat || (s == bestSat && d > bestDeg)) {
                bestSat = s;
                bestDeg = d;
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
