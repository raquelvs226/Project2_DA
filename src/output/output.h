#ifndef PROJECT1_OUTPUT_H
#define PROJECT1_OUTPUT_H

#include "data.h"

#include <string>
#include <vector>

using namespace std;

void writeOutputFile(
    const string& outputFile,
    const vector<Web>& webs,
    const AllocationResult& result
);


#endif //PROJECT1_OUTPUT_H