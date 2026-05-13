#include "splitting_allocation.h"
#include "basic_allocation.h"
#include "../graph_builder/graph_builder.h"

#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

// Find the split index (1-based, into the sorted points array) that maximises the
// number of interfering neighbours freed — i.e., neighbours that, after the split,
// only overlap with one of the two sub-webs and therefore lose one interference edge.
// Falls back to the median if no neighbour information is available.
static size_t findBestSplitIndex(
    const Web& web,
    const Graph<int>& g,
    const vector<Web>& allWebs
) {
    vector<ProgramPoint> sorted = web.points;
    sort(sorted.begin(), sorted.end(),
        [](const ProgramPoint& a, const ProgramPoint& b) { return a.line < b.line; });

    size_t n = sorted.size();
    if (n < 2) return 1;

    Vertex<int>* v = g.findVertex(web.id);
    if (!v) return n / 2;

    // Collect neighbour webs once
    vector<const Web*> neighbours;
    for (auto* edge : v->getAdj()) {
        int nid = edge->getDest()->getInfo();
        for (const auto& w : allWebs)
            if (w.id == nid) { neighbours.push_back(&w); break; }
    }

    size_t bestIdx  = n / 2;
    int    bestFreed = -1;

    for (size_t splitIdx = 1; splitIdx < n; splitIdx++) {
        set<int> linesA, linesB;
        for (size_t k = 0;        k < splitIdx; k++) linesA.insert(sorted[k].line);
        for (size_t k = splitIdx; k < n;        k++) linesB.insert(sorted[k].line);

        int freed = 0;
        for (const auto* nb : neighbours) {
            bool overA = false, overB = false;
            for (const auto& p : nb->points) {
                if (linesA.count(p.line)) overA = true;
                if (linesB.count(p.line)) overB = true;
            }
            if (overA != overB) freed++;   // neighbour freed from one side
        }
        if (freed > bestFreed) { bestFreed = freed; bestIdx = splitIdx; }
    }
    return bestIdx;
}

AllocationResult splittingAllocation(vector<Web>& webs, int numRegisters, int maxSplits) {

    // Start next ID just above the current maximum
    int nextId = 0;
    for (const auto& w : webs) nextId = max(nextId, w.id + 1);

    for (int splits = 0; splits <= maxSplits; splits++) {

        // Try basic allocation on a working copy
        vector<Web> tmp = webs;
        Graph<int>  g   = buildInterferenceGraph(tmp);
        AllocationResult res = basicAllocation(g, tmp, numRegisters);

        bool clean = all_of(tmp.begin(), tmp.end(),
            [](const Web& w) { return w.reg >= 0; });

        if (clean) {
            for (size_t i = 0; i < webs.size(); i++)
                webs[i].reg = tmp[i].reg;
            return res;
        }

        if (splits == maxSplits) break;

        // Find the highest-degree internally-spilled web with at least 2 points
        int bestId  = -1;
        int bestDeg = -1;
        for (size_t i = 0; i < tmp.size(); i++) {
            if (tmp[i].reg != -1)          continue;
            if (webs[i].points.size() < 2) continue;
            Vertex<int>* v = g.findVertex(webs[i].id);
            int deg = v ? (int)v->getAdj().size() : 0;
            if (deg > bestDeg) { bestDeg = deg; bestId = webs[i].id; }
        }

        if (bestId == -1) break;   // nothing splittable

        // Perform the split
        for (size_t i = 0; i < webs.size(); i++) {
            if (webs[i].id != bestId) continue;

            vector<ProgramPoint> sorted = webs[i].points;
            sort(sorted.begin(), sorted.end(),
                [](const ProgramPoint& a, const ProgramPoint& b) { return a.line < b.line; });

            size_t splitIdx = findBestSplitIndex(webs[i], g, webs);

            Web w1, w2;
            w1.id      = nextId++;
            w1.varName = webs[i].varName;
            w1.points  = { sorted.begin(), sorted.begin() + (ptrdiff_t)splitIdx };

            w2.id      = nextId++;
            w2.varName = webs[i].varName;
            w2.points  = { sorted.begin() + (ptrdiff_t)splitIdx, sorted.end() };

            webs.erase(webs.begin() + (ptrdiff_t)i);
            webs.insert(webs.begin() + (ptrdiff_t)i, w2);
            webs.insert(webs.begin() + (ptrdiff_t)i, w1);
            break;
        }
    }

    cout << "\nWarning: splitting allocation failed with " << numRegisters
         << " register(s) and at most " << maxSplits << " split(s). All webs assigned to memory.\n";
    AllocationResult fail;
    fail.usedRegisters = 0;
    for (auto& w : webs) w.reg = -1;
    return fail;
}
