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
        if (deg > bestDegree) {
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
    const auto& vertexSet = graph.getVertexSet();
    const int totalNodes  = vertexSet.size();

    // map web id → color assigned (-1 = not yet colored, -2 = spilled)
    unordered_map<int, int> color;
    for (auto v : vertexSet)
        color[v->getInfo()] = -1;

    set<int>   disabled;   // nodes removed in phase 1
    stack<int> nodeStack;  // order in which to color in phase 2

    // ── Phase 1: Simplification ──────────────────────────────────────────────
    while ((int)disabled.size() < totalNodes) {

        bool removed = false;

        // try to find a node with active degree < N
        for (auto v : vertexSet) {
            int id = v->getInfo();
            if (disabled.find(id) != disabled.end()) continue;

            if (activeDegree(v, disabled) < N) {
                disabled.insert(id);
                nodeStack.push(id);
                removed = true;
                // restart scan (degrees changed)
                break;
            }
        }

        // no node with degree < N found → must spill
        if (!removed) {
            int spillId = selectSpillNode(vertexSet, disabled);
            if (spillId == -1) break; // should not happen

            disabled.insert(spillId);
            color[spillId] = -2; // mark as spilled (do NOT push to stack)
        }
    }

    // ── Phase 2: Coloring ────────────────────────────────────────────────────
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

        color[id] = assigned; // assigned >= 0 always (degree was < N)
        if (assigned >= 0)
            usedColors.insert(assigned);
    }

    // ── Build result ─────────────────────────────────────────────────────────
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
