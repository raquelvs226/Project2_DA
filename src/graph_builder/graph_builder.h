#ifndef PROJECT1_GRAPH_BUILDER_H
#define PROJECT1_GRAPH_BUILDER_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

/**
 * @file graph_builder.h
 * @brief Functions for constructing the interference graph from a set of webs.
 */

/**
 * @brief Checks whether two webs interfere with each other.
 *
 * Two webs interfere if they share at least one program point that is not a
 * definition-use boundary. A definition-use boundary occurs when one web starts
 * (isStart) and the other ends (isEnd) at the same program point, meaning the
 * new definition replaces the old value without them coexisting.
 *
 * @param a First web.
 * @param b Second web.
 * @return true if the two webs interfere and cannot share a register.
 *
 * @note Complexity: O(|Pa| * |Pb|), where |Pa| and |Pb| are the number of
 * program points in each web.
 */
bool websInterfere(const Web& a, const Web& b);

/**
 * @brief Builds the undirected interference graph for a set of webs.
 *
 * Creates one vertex per web and adds a bidirectional edge between every pair
 * of webs that interfere according to websInterfere(). The resulting graph is
 * used as the input to the graph-coloring register allocation algorithms.
 *
 * @param webs List of webs to include as nodes in the graph.
 * @return Interference graph where nodes are web ids and edges denote conflicts.
 *
 * @note Complexity: O(W² * P²), where W is the number of webs and P is the
 * maximum number of program points in any single web.
 */
Graph<int> buildInterferenceGraph(const vector<Web>& webs);

#endif //PROJECT1_GRAPH_BUILDER_H
