#include "splitting_allocation.h"
#include "basic_allocation.h"
#include "../graph_builder/graph_builder.h"

#include <algorithm>
#include <climits>
#include <iostream>
#include <set>

using namespace std;

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
        Web tmpA, tmpB;
        tmpA.points = { sorted.begin(), sorted.begin() + static_cast<ptrdiff_t>(splitIdx) };
        tmpB.points = { sorted.begin() + static_cast<ptrdiff_t>(splitIdx), sorted.end() };

        int degA = 0, degB = 0;
        for (const auto* nb : neighbours) {
            if (websInterfere(tmpA, *nb)) degA++;
            if (websInterfere(tmpB, *nb)) degB++;
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

    vector<SplitRecord> splitLog;

    for (int splits = 0; splits <= maxSplits; splits++) {

        vector<Web> tmp = webs;
        Graph<int>  g   = buildInterferenceGraph(tmp);
        AllocationResult res = basicAllocation(g, tmp, numRegisters);

        bool clean = all_of(tmp.begin(), tmp.end(),
            [](const Web& w) { return w.reg >= 0; });

        if (clean) {
            for (auto& w : webs)
                for (const auto& tw : tmp)
                    if (tw.id == w.id) { w.reg = tw.reg; break; }
            res.splitLog = splitLog;
            return res;
        }

        if (splits == maxSplits) break;

        int    bestId       = -1;
        size_t bestSplitIdx = 0;
        int    bestMaxDeg   = INT_MAX;

        for (const auto& tw : tmp) {
            if (tw.reg != -1) continue;

            const Web* orig = nullptr;
            for (const auto& w : tmp)
                if (w.id == tw.id) { orig = &w; break; }
            if (!orig || orig->points.size() < 2) continue;

            auto [splitIdx, maxDeg] = findBestSplitPoint(*orig, g, tmp);

            if (maxDeg < bestMaxDeg) {
                bestMaxDeg   = maxDeg;
                bestId       = orig->id;
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

            int w1id = nextId++;
            int w2id = nextId++;
            int splitLine = sorted[bestSplitIdx - 1].line;

            Web w1, w2;
            w1.id      = w1id;
            w1.varName = webs[i].varName;
            w1.points  = { sorted.begin(), sorted.begin() + static_cast<ptrdiff_t>(bestSplitIdx) };

            w2.id      = w2id;
            w2.varName = webs[i].varName;
            w2.points  = { sorted.begin() + static_cast<ptrdiff_t>(bestSplitIdx), sorted.end() };

            splitLog.push_back({ bestId, w1id, w2id, splitLine });

            webs.erase(webs.begin() + static_cast<ptrdiff_t>(i));
            webs.insert(webs.begin() + static_cast<ptrdiff_t>(i), w2);
            webs.insert(webs.begin() + static_cast<ptrdiff_t>(i), w1);
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
