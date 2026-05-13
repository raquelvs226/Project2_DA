#include "splitting_allocation.h"
#include "basic_allocation.h"
#include "../graph_builder/graph_builder.h"

#include <algorithm>
#include <climits>
#include <iostream>
#include <set>

using namespace std;

// For every possible split of `web` into two contiguous halves (by sorted line number),
// compute deg(A) and deg(B) in the current interference graph.
// Returns the split index and the resulting min max(deg(A), deg(B)).
static pair<size_t, int> findBestSplitPoint(
    const Web& web,
    const Graph<int>& g,
    const vector<Web>& allWebs
) {
    vector<ProgramPoint> sorted = web.points;
    sort(sorted.begin(), sorted.end(),
        [](const ProgramPoint& a, const ProgramPoint& b) { return a.line < b.line; });

    size_t n = sorted.size();
    Vertex<int>* v = g.findVertex(web.id);

    if (n < 2 || !v) return { n / 2, INT_MAX };

    // Collect neighbours once
    vector<const Web*> neighbours;
    for (auto* edge : v->getAdj()) {
        int nid = edge->getDest()->getInfo();
        for (const auto& w : allWebs)
            if (w.id == nid) { neighbours.push_back(&w); break; }
    }

    size_t bestIdx    = n / 2;
    int    bestMaxDeg = INT_MAX;

    for (size_t splitIdx = 1; splitIdx < n; splitIdx++) {
        set<int> linesA, linesB;
        for (size_t k = 0;        k < splitIdx; k++) linesA.insert(sorted[k].line);
        for (size_t k = splitIdx; k < n;        k++) linesB.insert(sorted[k].line);

        int degA = 0, degB = 0;
        for (const auto* nb : neighbours) {
            bool overA = false, overB = false;
            for (const auto& p : nb->points) {
                if (linesA.count(p.line)) overA = true;
                if (linesB.count(p.line)) overB = true;
            }
            if (overA) degA++;
            if (overB) degB++;
        }

        int maxDeg = max(degA, degB);
        if (maxDeg < bestMaxDeg) {
            bestMaxDeg = maxDeg;
            bestIdx    = splitIdx;
        }
    }

    return { bestIdx, bestMaxDeg };
}

AllocationResult splittingAllocation(vector<Web>& webs, int numRegisters, int maxSplits) {

    int nextId = 0;
    for (const auto& w : webs) nextId = max(nextId, w.id + 1);

    for (int splits = 0; splits <= maxSplits; splits++) {

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

        // Among all internally-spilled webs with >= 2 points, pick the (web, split point)
        // pair that minimises max(deg(A), deg(B)) — i.e., the split that most relieves
        // interference pressure on both resulting sub-webs.
        int    bestId       = -1;
        size_t bestSplitIdx = 0;
        int    bestMaxDeg   = INT_MAX;

        for (size_t i = 0; i < tmp.size(); i++) {
            if (tmp[i].reg != -1)          continue;
            if (webs[i].points.size() < 2) continue;

            auto [splitIdx, maxDeg] = findBestSplitPoint(webs[i], g, webs);

            if (maxDeg < bestMaxDeg) {
                bestMaxDeg   = maxDeg;
                bestId       = webs[i].id;
                bestSplitIdx = splitIdx;
            }
        }

        if (bestId == -1) break;

        // Perform the split
        for (size_t i = 0; i < webs.size(); i++) {
            if (webs[i].id != bestId) continue;

            vector<ProgramPoint> sorted = webs[i].points;
            sort(sorted.begin(), sorted.end(),
                [](const ProgramPoint& a, const ProgramPoint& b) { return a.line < b.line; });

            Web w1, w2;
            w1.id      = nextId++;
            w1.varName = webs[i].varName;
            w1.points  = { sorted.begin(), sorted.begin() + (ptrdiff_t)bestSplitIdx };

            w2.id      = nextId++;
            w2.varName = webs[i].varName;
            w2.points  = { sorted.begin() + (ptrdiff_t)bestSplitIdx, sorted.end() };

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
