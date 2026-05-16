#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @file data.h
 * @brief Core data structures used throughout the register allocation tool.
 */

// ---------------- Program Point ----------------

/**
 * @struct ProgramPoint
 * @brief Represents a single execution point in the program where a variable is live.
 *
 * A program point corresponds to a specific source line number. The optional
 * flags indicate whether this is the definition (start) or the last use (end)
 * of the variable's live range at that line.
 *
 * @param line Source line number of this program point.
 * @param isStart True if the variable is defined (written) at this line.
 * @param isEnd True if this is the last use (read) of the variable at this line.
 */
struct ProgramPoint {
    int line;

    bool isStart = false;
    bool isEnd   = false;
};

// ---------------- Live Range ----------------

/**
 * @struct LiveRange
 * @brief Represents one contiguous live range of a variable as a sequence of program points.
 *
 * A variable may have multiple disjoint live ranges across the program. Each
 * LiveRange groups the program points belonging to one such range.
 *
 * @param points Ordered list of program points where the variable is live.
 */
struct LiveRange {
    vector<ProgramPoint> points;
};

// ---------------- Variable ----------------

/**
 * @struct Variable
 * @brief Represents a source-level variable and all of its live ranges.
 *
 * A variable may be live in multiple, possibly non-overlapping, regions of the
 * program. Each region is described by one LiveRange entry.
 *
 * @param name Unique name of the variable as it appears in the source.
 * @param ranges List of live ranges associated with this variable.
 */
struct Variable {
    string name;
    vector<LiveRange> ranges;
};

// ---------------- Web ----------------

/**
 * @struct Web
 * @brief Represents a web: the union of overlapping live ranges of one variable.
 *
 * A web is the maximal set of program points of a variable that can be held in
 * a single register simultaneously. It is the node of the interference graph.
 *
 * @param id Unique integer identifier of this web (used as the graph node key).
 * @param varName Name of the source variable this web belongs to.
 * @param points All program points that are part of this web.
 * @param reg Physical register assigned to this web (-1 if spilled to memory).
 */
struct Web {
    int id;
    string varName;
    vector<ProgramPoint> points;
    int reg = -1;
};

// ---------------- Register Config ----------------

/**
 * @struct Registers
 * @brief Configuration read from the registers input file.
 *
 * Specifies the number of available physical registers, the allocation
 * algorithm to run, and its optional numeric parameter.
 *
 * @param numRegisters Number of physical registers available.
 * @param algorithm Allocation algorithm identifier ("basic", "spilling",
 *        "splitting", or "free").
 * @param param Optional numeric parameter for the algorithm (e.g. max spills
 *        or max splits). Ignored when not applicable.
 */
struct Registers {
    int numRegisters = 0;
    string algorithm;
    int param = 0;
};

// ---------------- Output ----------------

/**
 * @struct SplitRecord
 * @brief Records one web-splitting operation performed during splitting allocation.
 *
 * @param originalId Web id of the web that was split.
 * @param partA Web id assigned to the first (earlier) sub-web.
 * @param partB Web id assigned to the second (later) sub-web.
 * @param splitLine Last program-point line of partA, marking the save/restore boundary.
 */
struct SplitRecord {
    int originalId;
    int partA;
    int partB;
    int splitLine;
};

/**
 * @struct AllocationResult
 * @brief Holds the outcome of a register allocation run.
 *
 * @param usedRegisters Number of distinct physical registers actually used.
 * @param webToRegister Map from web id to assigned register index for colored webs.
 * @param forcedSpills List of web ids that were explicitly chosen for memory spilling.
 * @param splitLog Ordered record of every split performed during splitting allocation.
 */
struct AllocationResult {
    int usedRegisters = 0;
    map<int, int> webToRegister;

    vector<int>         forcedSpills;
    vector<SplitRecord> splitLog;
};

#endif
