#ifndef PARSER_H
#define PARSER_H

#include "data.h"
#include <string>
#include <vector>

using namespace std;

class Parser {
public:
    // results (populated after parse + buildWebs)

    vector<LiveRange>  liveRanges; ///< Raw ranges from the ranges file
    vector<Web>        webs;       ///< Merged webs (one entry per web)
    RegisterConfig          config;     ///< Contents of the register config file

    // main entry points 

    void parseLiveRanges(const string& filename);

    void parseRegisterConfig(const string& filename);

    void buildWebs();

    // helpers (exposed for unit testing)

    static vector<ProgramPoint> parsePoints(const string& token);

    static string trim(const string& s);

private:
    static void mergeInto(Web& dst, const Web& src);
};

#endif // PARSER_H