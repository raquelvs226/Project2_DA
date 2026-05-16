#ifndef PROJECT1_WEBS_H
#define PROJECT1_WEBS_H

#include "data.h"

#include <vector>

using namespace std;

/**
 * @file webs.h
 * @brief Function for constructing live webs from variable live ranges.
 */

/**
 * @brief Builds the list of webs from the parsed variable live ranges.
 *
 * For each variable, starts with each live range as its own group and
 * repeatedly merges groups that share at least one program point until no
 * further merges are possible. Each remaining group becomes one Web.
 *
 * Variables are processed in alphabetical order so that web IDs are assigned
 * deterministically. Within each variable, groups are sorted by their minimum
 * program-point line before ID assignment, so IDs increase with earlier ranges.
 *
 * @param variables List of variables, each with one or more live ranges.
 * @return Vector of Web structs representing the merged live webs, with
 *         contiguous IDs starting from 0.
 *
 * @note Complexity: O(V * R² * P²), where V is the number of variables, R is
 * the maximum number of live ranges per variable, and P is the maximum number
 * of program points per range.
 */
vector<Web> buildWebs(const vector<Variable>& variables);

#endif //PROJECT1_WEBS_H
