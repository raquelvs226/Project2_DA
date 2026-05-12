#include "graph_builder.h"

#include <set>

using namespace std;

static bool isDefinitionUseBoundary(
    const ProgramPoint& a,
    const ProgramPoint& b
) {

    return
        (a.isStart && b.isEnd) || (a.isEnd && b.isStart);
}

bool websInterfere(const Web& a, const Web& b) {

    for (const auto& pa : a.points) {

        for (const auto& pb : b.points) {

            // same execution point
            if (pa.line == pb.line) {

                // special non-interference case
                if (isDefinitionUseBoundary(pa, pb)) {
                    continue;
                }

                return true;
            }
        }
    }

    return false;
}

Graph<int> buildInterferenceGraph(const vector<Web>& webs) {

    Graph<int> graph;

    // add one vertex per web
    for (const auto& web : webs) {
        graph.addVertex(web.id);
    }

    // build interference edges
    for (size_t i = 0; i < webs.size(); i++) {

        for (size_t j = i + 1; j < webs.size(); j++) {

            if (websInterfere(webs[i], webs[j])) {

                // undirected graph
                graph.addBidirectionalEdge(
                    webs[i].id,
                    webs[j].id,
                    1
                );
            }
        }
    }

    return graph;
}
