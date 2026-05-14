#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <map>

using namespace std;

// ---------------- Program Point ----------------

struct ProgramPoint {
    int line;

    bool isStart = false;
    bool isEnd   = false;
};

// ---------------- Live Range ----------------

struct LiveRange {
    vector<ProgramPoint> points;
};

// ---------------- Variable ----------------

struct Variable {
    string name;
    vector<LiveRange> ranges;
};

// ---------------- Web ----------------

struct Web {
    int id;
    string varName;
    vector<ProgramPoint> points;
    int reg = -1;
};

// ---------------- Register Config ----------------

struct Registers {
    int numRegisters = 0;
    string algorithm;
    int param = 0;
};

// ---------------- Output ----------------

struct SplitRecord {
    int originalId;   // web that was split
    int partA;        // first sub-web id
    int partB;        // second sub-web id
    int splitLine;    // last line of partA (save/restore point)
};

struct AllocationResult {
    int usedRegisters = 0;
    map<int, int> webToRegister;

    vector<int>        forcedSpills;  // web ids explicitly chosen for spilling
    vector<SplitRecord> splitLog;     // record of each split performed
};

#endif