#ifndef PROJECT2_DSATUR_ALLOCATION_H
#define PROJECT2_DSATUR_ALLOCATION_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

/**
 * @file dsatur_allocation.h
 * @brief DSatur graph-coloring register allocation with articulation-point heuristic.
 */

/**
 * @brief Performs register allocation using the DSatur algorithm with two tie-breaking heuristics.
 *
 * At each step, selects the uncolored web with the highest saturation (the number
 * of distinct colors already used by its colored neighbors) and assigns it the
 * lowest available color. Ties are broken by the following priority:
 * -# **Degree**: prefer the web with the highest total degree in the original graph.
 * -# **Articulation point**: prefer webs that are articulation points (cut vertices),
 *    as coloring structurally critical nodes earlier leaves them more color choices.
 *    Articulation points are pre-computed once via Tarjan's DFS in O(W + E).
 *
 * A web is spilled (reg = -1) only if all @p numRegisters colors are already
 * present among its colored neighbors when it is selected, meaning no valid
 * color exists.
 *
 * @param graph Interference graph where each node is a web id.
 * @param webs List of webs; the reg field of each web is updated in place.
 * @param numRegisters Number of available physical registers (colors).
 * @return AllocationResult with register assignments and the count of registers used.
 *
 * @note Complexity: O(W² + W * N + E), where W is the number of webs, E is the
 * number of interference edges, and N is the number of registers. The O(W²) term
 * comes from selecting the best node at each of the W steps by scanning all
 * uncolored nodes. Articulation point pre-computation adds O(W + E).
 */
AllocationResult dsaturAllocation(const Graph<int>& graph, vector<Web>& webs, int numRegisters);

#endif
