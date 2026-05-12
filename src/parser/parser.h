#ifndef PARSER_H
#define PARSER_H

#include "../data.h"

#include <string>
#include <vector>

using namespace std;

// utils
string trim(const string& str);
vector<string> split(const string& str, char delimiter);

// parsing
vector<Variable> parseRangesFile(const string& filename);
Registers parseRegistersFile(const string& filename);

#endif