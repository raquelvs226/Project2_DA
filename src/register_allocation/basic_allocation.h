#ifndef PROJECT1_BASIC_ALLOCATION_H
#define PROJECT1_BASIC_ALLOCATION_H

#include "data.h"
#include "graph/Graph.h"

#include <vector>

using namespace std;

AllocationResult basicAllocation(
    const Graph<int>& graph,
    vector<Web>& webs,
    int numRegisters
);

#endif //PROJECT1_BASIC_ALLOCATION_H