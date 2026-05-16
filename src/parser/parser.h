#ifndef PARSER_H
#define PARSER_H

#include "../data.h"

#include <string>
#include <vector>

using namespace std;

/**
 * @file parser.h
 * @brief Functions for parsing the ranges and registers input files.
 */

/**
 * @brief Removes leading and trailing whitespace from a string.
 *
 * @param str Input string.
 * @return A copy of @p str with leading and trailing spaces, tabs, and
 *         newline characters removed.
 */
string trim(const string& str);

/**
 * @brief Splits a string into tokens separated by a given delimiter.
 *
 * Each token is trimmed of whitespace before being added to the result.
 *
 * @param str Input string to split.
 * @param delimiter Character used as the separator.
 * @return Vector of trimmed tokens.
 */
vector<string> split(const string& str, char delimiter);

/**
 * @brief Parses a ranges input file and returns the list of variables with their live ranges.
 *
 * Each non-comment line has the form "varName: p1, p2, ..., pN" where each
 * program point is a line number optionally suffixed with '+' (definition/start)
 * or '-' (last use/end). Multiple lines with the same variable name each define
 * one live range; overlapping ranges of the same variable are merged into a
 * single web by buildWebs(). Lines beginning with '#' are ignored.
 *
 * @param filename Name of the ranges file (relative to "../../datasets/ranges/").
 * @return Vector of Variable structs, each containing all parsed live ranges.
 * @throws std::runtime_error if the file cannot be opened or a line is malformed.
 *
 */
vector<Variable> parseRangesFile(const string& filename);

/**
 * @brief Parses a registers input file and returns the allocation configuration.
 *
 * The file must contain a "registers: N" line specifying the number of physical
 * registers and an "algorithm: name[, param]" line specifying the algorithm.
 * Lines beginning with '#' are ignored.
 *
 * @param filename Name of the registers file (relative to "../../datasets/registers/").
 * @return Registers struct populated with the number of registers, algorithm name,
 *         and optional numeric parameter.
 * @throws std::runtime_error if the file cannot be opened or a line is malformed.
 *
 */
Registers parseRegistersFile(const string& filename);

#endif
