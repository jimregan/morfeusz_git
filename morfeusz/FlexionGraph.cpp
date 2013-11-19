
#include <string>
#include "utils.hpp"
#include "FlexionGraph.hpp"

FlexionGraph::FlexionGraph(int startNode)
: startNode(startNode) {
    
}

static inline void debugPath(const std::vector<InterpretedChunk>& path) {
    for (const InterpretedChunk& chunk: path) {
        std::string text(chunk.chunk, chunk.chunkLength);
        DEBUG(text);
        DEBUG(chunk.chunkLength);
    }
}

void FlexionGraph::addStartEdge(const Edge& e) {
    if (this->graph.empty()) {
        this->graph.push_back(vector<Edge>());
    }
    this->graph[0].push_back(e);
}
    
void FlexionGraph::addMiddleEdge(const Edge& e) {
    this->graph.push_back(vector<Edge>(1, e));
}

void FlexionGraph::addPath(const std::vector<InterpretedChunk>& path) {
//    debugPath(path);
    for (const InterpretedChunk& chunk: path) {
        if (&chunk == &(path.front()) 
                && &chunk == &(path.back())) {
            Edge e = { chunk, -1 };
            this->addStartEdge(e);
        }
        else if (&chunk == &(path.front())) {
            Edge e = { chunk, (int) this->graph.size() + 1 };
            this->addStartEdge(e);
        }
        else if (&chunk == &(path.back())) {
            Edge e = { chunk, -1 };
            this->addMiddleEdge(e);
        }
        else {
            Edge e = { chunk, (int) this->graph.size() + 1 };
            this->addMiddleEdge(e);
        }
    }
}

void FlexionGraph::minimizeGraph() {
    if (this->graph.size() > 2) {
        
    }
}

void FlexionGraph::appendToResults(const Tagset& tagset, std::vector<MorphInterpretation>& results) {
    this->minimizeGraph();
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

bool FlexionGraph::empty() const {
    return this->graph.empty();
}
