#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <set>

using namespace std;

//  Program Point

/**
 * @brief A single annotated program point within a live range.
 *
 * A line number can carry one of three annotations:
 *   DEF  (+)  – the variable is defined (written) at this line
 *   USE  (-)  – the variable is last used (read) at this line; range ends here
 *   NONE      – the variable is simply live at this line (no def/use marker)
 */
struct ProgramPoint {
    int line;

    enum class Kind { DEF, USE, NONE } kind = Kind::NONE;

    bool isDef() const { return kind == Kind::DEF; }
    bool isUse() const { return kind == Kind::USE; }
};

//  Live Range

struct LiveRange {
    std::string               varName; ///< Owner variable
    std::vector<ProgramPoint> points;  ///< Ordered list of program points
};

//  Web

struct Web {
    int            id;      ///< Unique sequential id (0-based)
    string    varName; ///< Originating variable name
    set<int>  lines;   ///< All program points covered by this web
    set<int>  defLines;///< Lines where this web has a DEF (+)
    set<int>  useLines;///< Lines where this web has a USE (-)

    bool interferesWith(const Web& other) const {
        for (int line : lines) {
            if (other.lines.count(line) == 0) continue;

            // Shared line found – check the non-interference exception:
            // A begins here with DEF and B ends here with USE  →  no conflict
            bool thisStartsDef  = defLines.count(line) > 0;
            bool otherEndsUse   = other.useLines.count(line) > 0;
            if (thisStartsDef && otherEndsUse) continue;

            bool otherStartsDef = other.defLines.count(line) > 0;
            bool thisEndsUse    = useLines.count(line) > 0;
            if (otherStartsDef && thisEndsUse) continue;

            return true; // genuine interference
        }
        return false;
    }
};

//  Algorithm config

enum class AlgorithmKind {
    BASIC,     ///< algorithm: basic
    SPILLING,  ///< algorithm: spilling, K
    SPLITTING, ///< algorithm: splitting, K
    FREE       ///< algorithm: free
};

struct RegisterConfig {
    int           numRegisters = 0;
    AlgorithmKind algorithm    = AlgorithmKind::BASIC;
    int           param        = 0; ///< K for spilling/splitting; 0 otherwise
};

#endif // DATA_H