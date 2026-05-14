#ifndef PROJECT2_DSATUR_ALLOCATION_H
#define PROJECT2_DSATUR_ALLOCATION_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

AllocationResult dsaturAllocation(const Graph<int>& graph, vector<Web>& webs, int numRegisters);

#endif
