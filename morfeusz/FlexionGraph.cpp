
#include <string>
#include <cassert>
#include <climits>
#include "utils.hpp"
#include "FlexionGraph.hpp"

static inline void debugPath(const std::vector<InterpretedChunk>& path) {
//    for (const InterpretedChunk& chunk : path) {
//        std::string text(chunk.chunkStartPtr, chunk.chunkLength);
//        DEBUG(text);
//        DEBUG(chunk.chunkLength);
//    }
}

void FlexionGraph::addStartEdge(const Edge& e) {
    if (this->graph.empty()) {
        assert(this->node2ChunkStartPtr.empty());
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.chunkStartPtr);
    }
    assert(this->node2ChunkStartPtr[0] == e.chunk.chunkStartPtr);
    this->graph[0].push_back(e);
}

void FlexionGraph::addMiddleEdge(const Edge& e) {
    this->graph.push_back(vector<Edge>(1, e));
    this->node2ChunkStartPtr.push_back(e.chunk.chunkStartPtr);
}

void FlexionGraph::addPath(const std::vector<InterpretedChunk>& path) {
    //    debugPath(path);
    for (const InterpretedChunk& chunk : path) {
        if (&chunk == &(path.front())
                && &chunk == &(path.back())) {
            Edge e = {chunk, UINT_MAX};
            this->addStartEdge(e);
        } else if (&chunk == &(path.front())) {
            Edge e = {chunk, this->graph.size() + 1};
            this->addStartEdge(e);
        } else if (&chunk == &(path.back())) {
            Edge e = {chunk, UINT_MAX};
            this->addMiddleEdge(e);
        } else {
            Edge e = {chunk, this->graph.size() + 1};
            this->addMiddleEdge(e);
        }
    }
}

bool FlexionGraph::canMergeNodes(unsigned int node1, unsigned int node2) {
    return this->node2ChunkStartPtr[node1] == this->node2ChunkStartPtr[node2] 
            && this->getPossiblePaths(node1) == this->getPossiblePaths(node2);
}

set<FlexionGraph::Path> FlexionGraph::getPossiblePaths(unsigned int node) {
    if (node == UINT_MAX || node == this->graph.size() - 1) {
        return set<FlexionGraph::Path>();
    }
    else {
        set<FlexionGraph::Path> res;
        for (Edge& e: this->graph[node]) {
            FlexionGraph::PathElement pathElem = make_pair(e.chunk.chunkStartPtr, e.chunk.interpsGroup.type);
            set<Path> nextPaths = this->getPossiblePaths(e.nextNode);
            for (Path path: nextPaths) {
                path.insert(pathElem);
            }
            res.insert(nextPaths.begin(), nextPaths.end());
        }
        return res;
    }
}

static bool containsEqualEdge(const vector<FlexionGraph::Edge>& edges, const FlexionGraph::Edge& e) {
    for (FlexionGraph::Edge e1: edges) {
        if (e1.chunk.chunkStartPtr == e.chunk.chunkStartPtr
                && e1.chunk.lowercaseCodepoints == e.chunk.lowercaseCodepoints
                && e1.chunk.interpsGroup.type == e.chunk.interpsGroup.type
                && e1.nextNode == e.nextNode) {
            return true;
        }
    }
    return false;
}

void FlexionGraph::redirectEdges(unsigned int fromNode, unsigned int toNode) {
    for (vector<Edge>& edges: this->graph) {
        for (Edge& e: edges) {
            if (e.nextNode == fromNode) {
                Edge newEdge = {e.chunk, toNode };
                if (!containsEqualEdge(edges, newEdge)) {
                    e.nextNode = toNode;
                }
            }
        }
    }
}

void FlexionGraph::doRemoveNode(unsigned int node) {
    for (unsigned int i = node + 1; i < this->graph.size(); i++) {
        redirectEdges(i, i - 1);
        this->graph[i - 1] = this->graph[i];
    }
    this->graph.pop_back();
}

void FlexionGraph::doMergeNodes(unsigned int node1, unsigned int node2) {
    if (node1 > node2) {
        doMergeNodes(node2, node1);
    }
    else {
        // node1 < node2
        for (Edge& e: this->graph[node2]) {
            if (!containsEqualEdge(graph[node1], e)) {
                this->graph[node1].push_back(e);
            }
        }
        this->redirectEdges(node2, node1);
        this->doRemoveNode(node2);
    }
}

bool FlexionGraph::tryToMergeTwoNodes() {
    for (unsigned int node1 = 0; node1 < this->graph.size(); node1++) {
        for (unsigned int node2 = 0; node2 < node1; node2++) {
            if (this->canMergeNodes(node1, node2)) {
                this->doMergeNodes(node1, node2);
                return true;
            }
        }
    }
    return false;
}

void FlexionGraph::minimizeGraph() {
    if (this->graph.size() > 2) {
        while (this->tryToMergeTwoNodes()) {
            
        }

    }
}

bool FlexionGraph::empty() const {
    return this->graph.empty();
}

void FlexionGraph::repairLastNodeNumbers() {
    for (vector<Edge>& edges: this->graph) {
        for (Edge& e: edges) {
            if (e.nextNode == UINT_MAX) {
                e.nextNode = this->graph.size();
            }
        }
    }
}
