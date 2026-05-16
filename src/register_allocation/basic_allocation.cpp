#include "basic_allocation.h"
#include <stack>
#include <set>
#include <unordered_map>
#include <algorithm>
using namespace std;

static int activeDegree(
    Vertex<int>* vertex,
    const set<int>& disabled
) {
    int degree = 0;
    for (auto edge : vertex->getAdj()) {
        int neighborId = edge->getDest()->getInfo();
        if (disabled.find(neighborId) == disabled.end())
            degree++;
    }
    return degree;
}

static int selectSpillNode(
    const vector<Vertex<int>*>& vertexSet,
    const set<int>& disabled
) {
    int bestId     = -1;
    int bestDegree = -1;
    for (auto v : vertexSet) {
        int id = v->getInfo();
        if (disabled.find(id) != disabled.end()) continue;
        int deg = activeDegree(v, disabled);
        if (deg > bestDegree || (deg == bestDegree && id < bestId)) {
            bestDegree = deg;
            bestId     = id;
        }
    }
    return bestId;
}

AllocationResult basicAllocation(
    const Graph<int>& graph,
    vector<Web>& webs,
    int numRegisters
) {
    AllocationResult result;
    const int N = numRegisters;
    auto vertexSet = graph.getVertexSet();
    sort(vertexSet.begin(), vertexSet.end(), [](Vertex<int>* a, Vertex<int>* b) {
        return a->getInfo() > b->getInfo();
    });
    const int totalNodes = (int)vertexSet.size();

    // map web id → color assigned (-1 = not yet colored, -2 = spilled)
    unordered_map<int, int> color;
    for (auto v : vertexSet)
        color[v->getInfo()] = -1;

    set<int>   disabled;   // nodes removed in phase 1
    stack<int> nodeStack;  // order in which to color in phase 2

    // ── Phase 1: Simplification ───────────────────────────────────────────────
    while ((int)disabled.size() < totalNodes) {
        bool removed = false;

        // keep removing nodes with active degree < N until none left
        bool removedAny = true;
        while (removedAny) {
            removedAny = false;
            for (auto v : vertexSet) {
                int id = v->getInfo();
                if (disabled.find(id) != disabled.end()) continue;
                if (activeDegree(v, disabled) < N) {
                    disabled.insert(id);
                    nodeStack.push(id);
                    removedAny = true;
                    removed    = true;
                }
            }
        }

        // no node with degree < N found → optimistic spill (push anyway)
        if (!removed) {
            int spillId = selectSpillNode(vertexSet, disabled);
            if (spillId == -1) break; // should not happen
            disabled.insert(spillId);
            nodeStack.push(spillId); // optimistic: still try to color in phase 2
        }
    }

    // ── Phase 2: Coloring ─────────────────────────────────────────────────────
    set<int> usedColors;
    while (!nodeStack.empty()) {
        int id = nodeStack.top();
        nodeStack.pop();

        Vertex<int>* v = graph.findVertex(id);
        if (v == nullptr) continue;

        // collect colors used by neighbors
        set<int> neighborColors;
        for (auto edge : v->getAdj()) {
            int neighborId = edge->getDest()->getInfo();
            if (color.count(neighborId) && color[neighborId] >= 0)
                neighborColors.insert(color[neighborId]);
        }

        // assign lowest color not used by neighbors
        int assigned = -1;
        for (int c = 0; c < N; c++) {
            if (neighborColors.find(c) == neighborColors.end()) {
                assigned = c;
                break;
            }
        }

        if (assigned == -1) {
            color[id] = -2;
        } else {
            color[id] = assigned;
            usedColors.insert(assigned);
        }
    }

    // ── Build result ──────────────────────────────────────────────────────────
    for (auto& web : webs) {
        int c = color[web.id];
        if (c >= 0) {
            web.reg = c;
            result.webToRegister[web.id] = c;
        } else {
            web.reg = -1; // spilled to memory
        }
    }
    result.usedRegisters = (int)usedColors.size();
    return result;
}