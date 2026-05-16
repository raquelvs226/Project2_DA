#ifndef PROJECT1_PIPELINE_H
#define PROJECT1_PIPELINE_H

#include "data.h"

#include <string>
#include <vector>

using namespace std;

/**
 * @file pipeline.h
 * @brief Orchestration of the full web construction and register allocation pipeline.
 */

/**
 * @brief Runs the complete register allocation pipeline.
 *
 * Orchestrates the workflow in the following order:
 * -# Constructs webs from the variable live ranges with buildWebs().
 * -# Dispatches to the algorithm specified in @p registers:
 *    - "basic": builds the interference graph and runs basicAllocation().
 *    - "spilling": runs spillingAllocation() with up to registers.param forced spills.
 *    - "splitting": runs splittingAllocation() with up to registers.param web splits.
 *    - "free": builds the interference graph and runs dsaturAllocation().
 * -# If basic allocation reports any spill, all webs are assigned to memory and
 *    a warning is printed to stdout.
 * -# Writes the result to @p outputFile with writeOutputFile().
 *
 * @param variables List of variables with their parsed live ranges.
 * @param registers Allocation configuration (number of registers, algorithm, param).
 * @param outputFile Name of the output file to write results to.
 * @throws std::runtime_error if an unknown algorithm name is specified.
 *
 */
void runPipeline(
    const vector<Variable>& variables,
    const Registers& registers,
    const string& outputFile
);

#endif
