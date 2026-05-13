#include "pipeline.h"

#include "../webs/webs.h"
#include "../graph/Graph.h"
#include "../graph_builder/graph_builder.h"
#include "../register_allocation/basic_allocation.h"
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

    // 2. Build interference graph
    Graph<int> graph = buildInterferenceGraph(webs);

    // 3. Register allocation (dispatch by algorithm)
    AllocationResult result;

    if (registers.algorithm == "basic") {
        result = basicAllocation(graph, webs, registers.numRegisters);

        bool anySpilled = any_of(webs.begin(), webs.end(),
            [](const Web& w) { return w.reg == -1; });

        if (anySpilled) {
            cerr << "Warning: register allocation with " << registers.numRegisters
                 << " register(s) is not possible.\n";
            markAllSpilled(webs, result);
        }
    }
    else if (registers.algorithm == "spilling") {
        // T2.2 — to be implemented
        result = basicAllocation(graph, webs, registers.numRegisters);
    }
    else if (registers.algorithm == "splitting") {
        // T2.3 — to be implemented
        result = basicAllocation(graph, webs, registers.numRegisters);
    }
    else if (registers.algorithm == "free") {
        // T2.4 — to be implemented
        result = basicAllocation(graph, webs, registers.numRegisters);
    }
    else {
        throw runtime_error("Unknown algorithm: " + registers.algorithm);
    }

    // 4. Output
    writeOutputFile(outputFile, webs, result);
}