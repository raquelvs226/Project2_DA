#ifndef PROJECT1_SPILLING_ALLOCATION_H
#define PROJECT1_SPILLING_ALLOCATION_H

#include "data.h"
#include <vector>

using namespace std;

/**
 * @file spilling_allocation.h
 * @brief Register allocation with iterative web spilling.
 */

/**
 * @brief Performs register allocation by iteratively forcing high-degree webs to memory.
 *
 * Attempts to color the interference graph using basicAllocation(). If the basic
 * algorithm reports any internal spills, the highest-degree spilled web is chosen
 * and permanently forced to memory (removed from the active set). This process
 * repeats up to @p maxSpills times until a clean coloring is found or the limit
 * is reached.
 *
 * If a clean coloring is found, all forced-spill web ids are recorded in
 * AllocationResult::forcedSpills and those webs receive reg = -1. If no clean
 * coloring is achieved within the limit, all webs are assigned to memory and a
 * warning is printed to stdout.
 *
 * @param webs List of webs; the reg field of each web is updated in place.
 * @param numRegisters Number of available physical registers.
 * @param maxSpills Maximum number of webs that may be explicitly spilled.
 * @return AllocationResult with register assignments, used register count,
 *         and the list of forced-spill web ids.
 *
 * @note Complexity: O(K * (W² * P² + W² + W * E)), where K = maxSpills, W is
 * the number of webs, P is the maximum number of program points per web, and
 * E is the number of edges in the interference graph. Each iteration rebuilds
 * the interference graph and runs basicAllocation() on the active subset.
 */
AllocationResult spillingAllocation(vector<Web>& webs, int numRegisters, int maxSpills);

#endif
