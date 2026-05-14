#include "webs.h"

#include <set>
#include <algorithm>
#include <climits>

using namespace std;

static bool rangesOverlap(const LiveRange& a, const LiveRange& b) {
    for (const auto& pa : a.points)
        for (const auto& pb : b.points)
            if (pa.line == pb.line)
                return true;
    return false;
}

static LiveRange mergeRanges(const LiveRange& a, const LiveRange& b) {
    LiveRange merged = a;

    for (const auto& pb : b.points) {
        bool found = false;
        for (auto& pa : merged.points) {
            if (pa.line == pb.line) {
                // keep flags from both
                if (pb.isStart) pa.isStart = true;
                if (pb.isEnd)   pa.isEnd   = true;
                found = true;
                break;
            }
        }
        if (!found)
            merged.points.push_back(pb);
    }
    return merged;
}

vector<Web> buildWebs(const vector<Variable>& variables) {

    vector<Web> webs;
    int currentId = 0;

    // sort variables alphabetically so web IDs are assigned in a deterministic order
    vector<Variable> sorted = variables;
    sort(sorted.begin(), sorted.end(), [](const Variable& a, const Variable& b) {
        return a.name < b.name;
    });

    for (const auto& variable : sorted) {

        // start with each range as its own group
        vector<LiveRange> groups = variable.ranges;

        // repeatedly merge overlapping groups until stable
        bool merged = true;
        while (merged) {
            merged = false;
            for (size_t i = 0; i < groups.size(); i++) {
                for (size_t j = i + 1; j < groups.size(); j++) {
                    if (rangesOverlap(groups[i], groups[j])) {
                        // merge j into i
                        groups[i] = mergeRanges(groups[i], groups[j]);
                        groups.erase(groups.begin() + j);
                        merged = true;
                        break;
                    }
                }
                if (merged) break;
            }
        }

        // sort groups by minimum program point so IDs are assigned in ascending order
        sort(groups.begin(), groups.end(), [](const LiveRange& a, const LiveRange& b) {
            int minA = INT_MAX, minB = INT_MAX;
            for (const auto& p : a.points) minA = min(minA, p.line);
            for (const auto& p : b.points) minB = min(minB, p.line);
            return minA < minB;
        });

        // each remaining group becomes a web
        for (const auto& group : groups) {
            Web web;
            web.id      = currentId++;
            web.varName = variable.name;
            web.points  = group.points;
            webs.push_back(web);
        }
    }

    return webs;
}