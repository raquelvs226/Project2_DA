#ifndef PROJECT1_SPLITTING_ALLOCATION_H
#define PROJECT1_SPLITTING_ALLOCATION_H

#include "data.h"
#include <vector>

using namespace std;

/**
 * @file splitting_allocation.h
 * @brief Register allocation with iterative web splitting.
 */

/**
 * @brief Performs register allocation by iteratively splitting high-interference webs.
 *
 * Attempts to color the interference graph using basicAllocation(). If any web
 * is internally spilled, the algorithm selects the split that minimises the
 * maximum degree of the two resulting sub-webs across all spilled webs and all
 * possible contiguous split points. The chosen web is replaced by two sub-webs
 * (partA covering the earlier program points, partB covering the later ones),
 * a save/restore boundary is inserted at the split line, and the process repeats.
 *
 * This continues until a clean coloring is achieved or @p maxSplits splits have
 * been performed. Each split is recorded in AllocationResult::splitLog. If no
 * clean coloring is found within the limit, all webs are assigned to memory and
 * a warning is printed to stdout.
 *
 * @param webs List of webs; modified in place (split sub-webs replace the original).
 *             The reg field of each remaining web is updated after a successful coloring.
 * @param numRegisters Number of available physical registers.
 * @param maxSplits Maximum number of web splits to attempt.
 * @return AllocationResult with register assignments, used register count,
 *         and the ordered split log.
 *
 * @note Complexity: O(K * (W² * P² + W * P * deg)), where K = maxSplits, W is
 * the number of webs (grows by one per split), P is the maximum number of program
 * points per web, and deg is the maximum degree in the interference graph.
 * Finding the best split point for one web costs O(P * deg) (trying each split
 * index and recomputing interference with all neighbors).
 */
AllocationResult splittingAllocation(vector<Web>& webs, int numRegisters, int maxSplits);

#endif
