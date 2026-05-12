#include "webs.h"

using namespace std;

vector<Web> buildWebs(const vector<Variable>& variables) {

    vector<Web> webs;

    int currentId = 0;

    for (const auto& variable : variables) {

        for (const auto& range : variable.ranges) {

            Web web;

            web.id = currentId++;
            web.varName = variable.name;
            web.points = range.points;

            webs.push_back(web);
        }
    }

    return webs;
}