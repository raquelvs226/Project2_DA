#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

using namespace std;

struct ProgramPoint {
    int line;
    bool isStart;
    bool isEnd;
};

struct LiveRange {
    vector<ProgramPoint> points;
};

struct Variable {
    string name;
    vector<LiveRange> ranges;
};

// agora chama-se Registers (antes Config)
struct Registers {
    int registers = 0;
    string algorithm;
    int parameter = 0;
};

// utils
string trim(const string& str);
vector<string> split(const string& str, char delimiter);

// parsing
vector<Variable> parseRangesFile(const string& filename);
Registers parseRegistersFile(const string& filename);

#endif