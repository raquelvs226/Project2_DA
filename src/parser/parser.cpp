#include "parser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <map>

using namespace std;

// Utility

std::string Parser::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

// parsePoints

vector<ProgramPoint> Parser::parsePoints(const std::string& token) {
    vector<ProgramPoint> result;
    istringstream ss(token);
    string item;

    while (getline(ss, item, ',')) {
        item = trim(item);
        if (item.empty()) continue;

        ProgramPoint pp;

        char last = item.back();
        if (last == '+') {
            pp.kind = ProgramPoint::Kind::DEF;
            item.pop_back();
        } else if (last == '-') {
            pp.kind = ProgramPoint::Kind::USE;
            item.pop_back();
        } else {
            pp.kind = ProgramPoint::Kind::NONE;
        }

        try {
            pp.line = std::stoi(item);
        } catch (...) {
            throw runtime_error("Invalid line number in point list: '" + item + "'");
        }

        result.push_back(pp);
    }

    return result;
}

// parseLiveRanges

void Parser::parseLiveRanges(const string& filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open live-ranges file: " + filename);

    liveRanges.clear();
    string line;
    int lineNum = 0;

    while (getline(file, line)) {
        ++lineNum;
        line = trim(line);

        // skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        // split at the first ':'
        size_t colon = line.find(':');
        if (colon == string::npos)
            throw runtime_error("Missing ':' on line " + to_string(lineNum)
                                     + " of " + filename);

        LiveRange lr;
        lr.varName = trim(line.substr(0, colon));
        std::string pointsStr = trim(line.substr(colon + 1));

        if (lr.varName.empty())
            throw runtime_error("Empty variable name on line " + to_string(lineNum));

        lr.points = parsePoints(pointsStr);

        if (lr.points.empty())
            throw runtime_error("No program points for variable '"
                                     + lr.varName + "' on line " + to_string(lineNum));

        liveRanges.push_back(move(lr));
    }
}


void Parser::parseRegisterConfig(const string& filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open register config file: " + filename);

    config = RegisterConfig{}; // reset
    std::string line;
    int lineNum = 0;

    while (getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colon = line.find(':');
        if (colon == string::npos)
            throw runtime_error("Missing ':' on line " + to_string(lineNum)
                                     + " of " + filename);

        string key   = trim(line.substr(0, colon));
        string value = trim(line.substr(colon + 1));

        if (key == "registers") {
            try {
                config.numRegisters = stoi(value);
            } catch (...) {
                throw runtime_error("Invalid register count: '" + value + "'");
            }
            if (config.numRegisters < 0)
                throw runtime_error("Register count cannot be negative.");

        } else if (key == "algorithm") {
            // value is e.g. "basic", "spilling, 2", "splitting, 3", "free"
            size_t comma = value.find(',');
            string algName = trim(value.substr(0, comma));

            if (algName == "basic") {
                config.algorithm = AlgorithmKind::BASIC;
                config.param     = 0;

            } else if (algName == "spilling") {
                config.algorithm = AlgorithmKind::SPILLING;
                if (comma == string::npos)
                    throw runtime_error("'spilling' requires a numeric parameter K.");
                try {
                    config.param = std::stoi(trim(value.substr(comma + 1)));
                } catch (...) {
                    throw runtime_error("Invalid spilling parameter K.");
                }

            } else if (algName == "splitting") {
                config.algorithm = AlgorithmKind::SPLITTING;
                if (comma == string::npos)
                    throw runtime_error("'splitting' requires a numeric parameter K.");
                try {
                    config.param = std::stoi(trim(value.substr(comma + 1)));
                } catch (...) {
                    throw runtime_error("Invalid splitting parameter K.");
                }

            } else if (algName == "free") {
                config.algorithm = AlgorithmKind::FREE;
                config.param     = 0;

            } else {
                throw runtime_error("Unknown algorithm: '" + algName + "'");
            }
        }
        // unknown keys are silently ignored (forward-compatibility)
    }
}

// mergeInto  (private helper)

void Parser::mergeInto(Web& dst, const Web& src) {
    dst.lines.insert(src.lines.begin(), src.lines.end());
    dst.defLines.insert(src.defLines.begin(), src.defLines.end());
    dst.useLines.insert(src.useLines.begin(), src.useLines.end());
}

// buildWebs

static Web liveRangeToWeb(const LiveRange& lr, int id) {
    Web w;
    w.id      = id;
    w.varName = lr.varName;
    for (const auto& pp : lr.points) {
        w.lines.insert(pp.line);
        if (pp.isDef()) w.defLines.insert(pp.line);
        if (pp.isUse()) w.useLines.insert(pp.line);
    }
    return w;
}

static bool shouldFuse(const Web& a, const Web& b) {
    // Condition 1: any common line
    for (int line : a.lines) {
        if (b.lines.count(line)) return true;
    }

    // Condition 2: a USE in a coincides with a DEF in b (or vice-versa)
    for (int uLine : a.useLines) {
        if (b.defLines.count(uLine)) return true;
    }
    for (int uLine : b.useLines) {
        if (a.defLines.count(uLine)) return true;
    }

    return false;
}

void Parser::buildWebs() {
    webs.clear();

    // Group live ranges by variable name
    map<string, vector<LiveRange>> byVar;
    for (const auto& lr : liveRanges)
        byVar[lr.varName].push_back(lr);

    int nextId = 0;

    for (auto& [varName, ranges] : byVar) {
        // Convert to preliminary webs
        std::vector<Web> varWebs;
        varWebs.reserve(ranges.size());
        for (const auto& lr : ranges)
            varWebs.push_back(liveRangeToWeb(lr, nextId++));

        // Greedy fixed-point merge
        bool merged = true;
        while (merged) {
            merged = false;
            for (size_t i = 0; i < varWebs.size() && !merged; ++i) {
                for (size_t j = i + 1; j < varWebs.size(); ++j) {
                    if (shouldFuse(varWebs[i], varWebs[j])) {
                        mergeInto(varWebs[i], varWebs[j]);
                        varWebs.erase(varWebs.begin() + j);
                        merged = true;
                        break; // restart inner loop
                    }
                }
            }
        }

        for (auto& w : varWebs)
            webs.push_back(move(w));
    }

    // Re-number all webs sequentially from 0
    for (int i = 0; i < static_cast<int>(webs.size()); ++i)
        webs[i].id = i;
}