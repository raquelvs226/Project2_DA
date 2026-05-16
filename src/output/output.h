#ifndef PROJECT1_OUTPUT_H
#define PROJECT1_OUTPUT_H

#include "data.h"

#include <string>
#include <vector>

using namespace std;

/**
 * @file output.h
 * @brief Function for writing the register allocation results to an output file.
 */

/**
 * @brief Writes the register allocation results to a text file.
 *
 * The output file is written to "../../datasets/results/" and contains:
 * -# A webs section listing all webs with their program points sorted in
 *    ascending order.
 * -# A registers section mapping each web to its assigned register (r0..rN-1)
 *    or to memory (M) if it was spilled. If the allocation failed entirely,
 *    all webs are mapped to M and usedRegisters is 0.
 * -# An optional spills section (only present for spilling allocation) listing
 *    the webs that were explicitly chosen for memory spilling.
 * -# An optional splits section (only present for splitting allocation) listing
 *    each split performed, the two resulting sub-webs, and the save/restore line.
 *
 * @param outputFile Name of the output file (relative to the results directory).
 * @param webs List of all webs produced by the allocation (including split sub-webs).
 * @param result Allocation result containing register assignments, spill info, and split log.
 *
 */
void writeOutputFile(
    const string& outputFile,
    const vector<Web>& webs,
    const AllocationResult& result
);

#endif //PROJECT1_OUTPUT_H
