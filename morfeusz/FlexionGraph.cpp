
#include "FlexionGraph.hpp"

FlexionGraph::FlexionGraph(int startNode)
: startNode(startNode) {
    
}

void FlexionGraph::addPath(const std::vector<InterpretedChunk>& path) {
    for (const InterpretedChunk& chunk: path) {
        if (&chunk == &(path.back())) {
            Edge e = { chunk, -1 };
            vector<Edge> v;
            v.push_back(e);
            this->graph.push_back(v);
//            this->graph[node].push_back(e);
        }
        else if (&chunk == &(path.front())) {
            Edge e = { chunk, (int) this->graph.size() };
            this->graph[0].push_back(e);
        }
        else {
            Edge e = { chunk, (int) this->graph.size() };
            vector<Edge> v;
            v.push_back(e);
            this->graph.push_back(v);
        }
    }
}

void FlexionGraph::appendToResults(const Tagset& tagset, std::vector<MorphInterpretation>& results) {
    int endNode = graph.size();
    for (unsigned int i = 0; i < graph.size(); i++) {
        vector<Edge>& edges = graph[i];
        for (Edge& e: edges) {
            int realStartNode = i + this->startNode;
            int realEndNode = e.nextNode == -1 ? (endNode + this->startNode) : (i + e.nextNode);
            string orth(e.chunk.chunk, e.chunk.chunkLength);
            vector<MorphInterpretation> interps = e.chunk.interpsGroup.getRealInterps(orth, realStartNode, realEndNode, tagset);
            results.insert(results.end(), interps.begin(), interps.end());
        }
    }
}
