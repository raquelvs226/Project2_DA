#ifndef PROJECT1_SPILLING_ALLOCATION_H
#define PROJECT1_SPILLING_ALLOCATION_H

#include "data.h"
#include <vector>

using namespace std;

AllocationResult spillingAllocation(vector<Web>& webs, int numRegisters, int maxSpills);

#endif
