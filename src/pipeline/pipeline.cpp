#include "pipeline.h"

#include "../webs/webs.h"
#include "../graph/Graph.h"
#include "../graph_builder/graph_builder.h"
#include "../register_allocation/basic_allocation.h"
#include "../output/output.h"

using namespace std;

void runPipeline(
    const vector<Variable>& variables,
    const Registers& registers,
    const string& outputFile
) {

    // 1. Build webs
    vector<Web> webs =
        buildWebs(variables);

    //2. Build interference graph (GRAPH DO PROF)
    Graph<int> graph =
        buildInterferenceGraph(webs);

    // 3. Register allocation
    AllocationResult result =
        basicAllocation(
            graph,
            webs,
            registers.numRegisters
        );

    // 4. Output
    writeOutputFile(
        outputFile,
        webs,
        result
    );
}