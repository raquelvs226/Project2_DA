#ifndef PROJECT1_PIPELINE_H
#define PROJECT1_PIPELINE_H

#include "data.h"

#include <string>
#include <vector>

using namespace std;

void runPipeline(
    const vector<Variable>& variables,
    const Registers& registers,
    const string& outputFile
);

#endif