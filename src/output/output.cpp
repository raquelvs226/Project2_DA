#include "output.h"

#include <fstream>
#include <algorithm>
#include <map>
#include <iostream>

using namespace std;

static string formatWebPoints(const Web& web) {

    // sort points by line number
    vector<ProgramPoint> sorted = web.points;
    sort(sorted.begin(), sorted.end(), [](const ProgramPoint& a, const ProgramPoint& b) {
        return a.line < b.line;
    });

    string result;
    for (size_t i = 0; i < sorted.size(); i++) {
        if (i > 0) result += ",";

        result += to_string(sorted[i].line);

        if (sorted[i].isStart) result += "+";
        else if (sorted[i].isEnd) result += "-";
    }
    return result;
}

void writeOutputFile(
    const string& outputFile,
    const vector<Web>& webs,
    const AllocationResult& result
) {
    string filePath = "../../datasets/results/" + outputFile;
    ofstream file(filePath);

    if (!file.is_open())
        throw runtime_error("Could not open output file: " + outputFile);

    bool allSpilled = result.usedRegisters == 0;

    // ── Webs section ─────────────────────────────────────────────────────────
    file << "# Total number of webs followed by the listing of the program points of each one\n";
    file << "# program points in each web are sorted in ascending order\n";
    file << "webs: " << webs.size() << "\n";

    for (const auto& web : webs) {
        file << "web" << web.id << ": " << formatWebPoints(web) << "\n";
    }

    file << "\n";

    // ── Registers section ─────────────────────────────────────────────────────
    file << "# Total number of registers used, followed by assignment to webs\n";
    file << "registers: " << result.usedRegisters << "\n";

    if (allSpilled) {
        for (const auto& web : webs)
            file << "M: web" << web.id << "\n";
    } else {
        // group webs by register: r0 → [web0, web2], r1 → [web1], M → [web3]
        map<int, vector<int>> regToWebs; // reg → list of web ids
        vector<int> spilledWebs;

        for (const auto& web : webs) {
            if (web.reg >= 0)
                regToWebs[web.reg].push_back(web.id);
            else
                spilledWebs.push_back(web.id);
        }

        // print registers
        for (const auto& [reg, webIds] : regToWebs) {
            for (int webId : webIds)
                file << "r" << reg << ": web" << webId << "\n";
        }

        // print spilled webs
        for (int webId : spilledWebs)
            file << "M: web" << webId << "\n";
    }

    file.close();
}
