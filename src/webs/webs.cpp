#include "webs.h"

#include <set>
#include <algorithm>

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

    for (const auto& variable : variables) {

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