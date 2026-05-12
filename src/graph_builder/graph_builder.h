#ifndef PROJECT1_GRAPH_BUILDER_H
#define PROJECT1_GRAPH_BUILDER_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

Graph<int> buildInterferenceGraph(const vector<Web>& webs);

bool websInterfere(const Web& a, const Web& b);

#endif //PROJECT1_GRAPH_BUILDER_H
