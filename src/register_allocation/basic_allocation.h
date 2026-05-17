#ifndef PROJECT1_BASIC_ALLOCATION_H
#define PROJECT1_BASIC_ALLOCATION_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

/**
 * @file basic_allocation.h
 * @brief Chaitin-style graph-coloring register allocation algorithm.
 */

/**
 * @brief Performs register allocation using the basic Chaitin simplification algorithm.
 *
 * Runs in two phases:
 * -# **Simplification**: repeatedly removes all nodes whose active degree is
 *    strictly less than @p numRegisters and pushes them onto a stack. If no such
 *    node exists, the highest-degree active node is selected as a spill candidate
 *    and pushed onto the stack optimistically (ties broken by smallest web id).
 * -# **Coloring**: pops nodes from the stack and assigns each the lowest color
 *    not used by any already-colored neighbor. Optimistic spill candidates that
 *    cannot be colored (all @p numRegisters colors taken by neighbors) receive
 *    reg = -1 and are spilled to memory.
 *
 * If any web is spilled, the pipeline marks all webs as spilled.
 *
 * @param graph Interference graph where each node is a web id.
 * @param webs List of webs; the reg field of each web is updated in place.
 * @param numRegisters Number of available physical registers (colors).
 * @return AllocationResult with register assignments and the count of registers used.
 *
 * @note Complexity: O(W * E), where W is the number of webs and E is the
 * number of edges in the interference graph. Each of the W simplification steps
 * scans all remaining active nodes and their adjacency lists to compute degrees.
 */
AllocationResult basicAllocation(
    const Graph<int>& graph,
    vector<Web>& webs,
    int numRegisters
);

#endif //PROJECT1_BASIC_ALLOCATION_H
