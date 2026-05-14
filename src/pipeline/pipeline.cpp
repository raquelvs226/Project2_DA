#include "pipeline.h"

#include "../webs/webs.h"
#include "../graph/Graph.h"
#include "../graph_builder/graph_builder.h"
#include "../register_allocation/basic_allocation.h"
#include "../register_allocation/spilling_allocation.h"
#include "../register_allocation/splitting_allocation.h"
#include "../register_allocation/dsatur_allocation.h"
#include "../output/output.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace std;

static void markAllSpilled(vector<Web>& webs, AllocationResult& result) {
    result.usedRegisters = 0;
    result.webToRegister.clear();
    for (auto& web : webs)
        web.reg = -1;
}

void runPipeline(
    const vector<Variable>& variables,
    const Registers& registers,
    const string& outputFile
) {
    // 1. Build webs
    vector<Web> webs = buildWebs(variables);

    // 3. Register allocation (dispatch by algorithm)
    AllocationResult result;

    if (registers.algorithm == "basic") {
        Graph<int> graph = buildInterferenceGraph(webs);
        result = basicAllocation(graph, webs, registers.numRegisters);

        bool anySpilled = any_of(webs.begin(), webs.end(),
            [](const Web& w) { return w.reg == -1; });

        if (anySpilled) {
            cout << "\nWarning: basic allocation failed with " << registers.numRegisters
                 << " register(s). All webs assigned to memory.\n";
            markAllSpilled(webs, result);
        }
    }
    else if (registers.algorithm == "spilling") {
        result = spillingAllocation(webs, registers.numRegisters, registers.param);
    }
    else if (registers.algorithm == "splitting") {
        result = splittingAllocation(webs, registers.numRegisters, registers.param);
    }
    else if (registers.algorithm == "free") {
        Graph<int> graph = buildInterferenceGraph(webs);
        result = dsaturAllocation(graph, webs, registers.numRegisters);
    }
    else {
        throw runtime_error("Unknown algorithm: " + registers.algorithm);
    }

    // 4. Output
    writeOutputFile(outputFile, webs, result);
}