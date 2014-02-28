
#include <string>
#include <cassert>
#include <climits>
#include <vector>
#include "utils.hpp"
#include "FlexionGraph.hpp"

using namespace std;

void FlexionGraph::addStartEdge(const Edge& e) {
    if (this->graph.empty()) {
        assert(this->node2ChunkStartPtr.empty());
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.chunkStartPtr);
    }
    assert(this->node2ChunkStartPtr[0] == e.chunk.chunkStartPtr);
    this->graph[0].push_back(e);
}

void FlexionGraph::addMiddleEdge(unsigned int startNode, const Edge& e) {
    assert(startNode < e.nextNode);
    assert(startNode == this->graph.size());
    if (startNode == this->graph.size()) {
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.chunkStartPtr);
    }
    this->graph[startNode].push_back(e);
}

void FlexionGraph::addPath(const std::vector<InterpretedChunk>& path) {
    //    debugPath(path);
    //    debugGraph(this->graph);
    for (unsigned int i = 0; i < path.size(); i++) {
        const InterpretedChunk& chunk = path[i];
        if (!chunk.shiftOrth) {
            if (&chunk == &(path.front())
                    && &chunk == &(path.back())) {
                Edge e = {chunk, UINT_MAX};
                this->addStartEdge(e);
            } else if (&chunk == &(path.front())) {
                Edge e = {chunk, this->graph.empty() ? 1 : (unsigned int) this->graph.size()};
                this->addStartEdge(e);
            } else if (&chunk == &(path.back())) {
                Edge e = {chunk, UINT_MAX};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            } else {
                Edge e = {chunk, (int) this->graph.size() + 1};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            }
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
    } else {
        set<FlexionGraph::Path> res;
        vector<Edge>& edges = this->graph.at(node);
        for (unsigned int i = 0; i < edges.size(); i++) {
            Edge& e = edges[i];
            FlexionGraph::PathElement pathElem(e.chunk.chunkStartPtr, e.chunk.interpsGroup.type);
            if (e.nextNode != this->graph.size()) {
                set<Path> possiblePaths = this->getPossiblePaths(e.nextNode);
                vector<Path> nextPaths(possiblePaths.begin(), possiblePaths.end());
                vector<Path>::iterator it;
                for (it = nextPaths.begin(); it != nextPaths.end(); ++it) {
                    (*it).insert(pathElem);
                }
                res.insert(nextPaths.begin(), nextPaths.end());
            }
        }
        return res;
    }
}

static bool containsEqualEdge(const vector<FlexionGraph::Edge>& edges, const FlexionGraph::Edge& e) {
    for (unsigned int i = 0; i < edges.size(); i++) {
        const FlexionGraph::Edge& e1 = edges[i];
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
    for (unsigned int node = 0; node < fromNode; node++) {
        vector<Edge>& edges = this->graph[node];
        vector<Edge>::iterator edgesIt = edges.begin();
        while (edgesIt != edges.end()) {
            Edge& oldEdge = *edgesIt;
            if (oldEdge.nextNode == fromNode) {
                Edge newEdge = {oldEdge.chunk, toNode};
                if (!containsEqualEdge(edges, newEdge)) {
                    // if newEdge is not in edges, redirect edgeEdge
                    // so it becomes newEdge
                    oldEdge.nextNode = toNode;
                } else {
                    // if newEdge is already there, just remove old edge
                    edges.erase(edgesIt);
                }
            } else {
                ++edgesIt;
            }
        }
    }
}

void FlexionGraph::doRemoveNode(unsigned int node) {
    for (unsigned int i = node + 1; i < this->graph.size(); i++) {
        redirectEdges(i, i - 1);
        this->graph[i - 1] = this->graph[i];
        this->node2ChunkStartPtr[i - 1] = this->node2ChunkStartPtr[i];
    }
    this->graph.pop_back();
    this->node2ChunkStartPtr.pop_back();
}

void FlexionGraph::doMergeNodes(unsigned int node1, unsigned int node2) {
    if (node1 > node2) {
        doMergeNodes(node2, node1);
    } else {
        // node1 < node2
        for (unsigned int i = 0; i < this->graph[node2].size(); i++) {
            Edge& e = this->graph[node2][i];
            if (!containsEqualEdge(graph[node1], e)) {
                this->graph[node1].push_back(e);
            }
        }
        //        DEBUG("1");
        //        debugGraph(this->graph);
        this->redirectEdges(node2, node1);
        //        DEBUG("2");
        //        debugGraph(this->graph);
        this->doRemoveNode(node2);
        //        DEBUG("3");
        //        debugGraph(this->graph);
    }
}

bool FlexionGraph::tryToMergeTwoNodes() {
    for (unsigned int node1 = 0; node1 < this->graph.size(); node1++) {
        for (unsigned int node2 = this->graph.size() - 1; node2 > node1; node2--) {
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
        //        debugGraph(this->graph);
        while (this->tryToMergeTwoNodes()) {
            //            debugGraph(this->graph);
        }
    }
}

bool FlexionGraph::empty() const {
    return this->graph.empty();
}

void FlexionGraph::repairLastNodeNumbers() {
    for (unsigned int i = 0; i < this->graph.size(); i++) {
        vector<Edge>& edges = this->graph[i];
        for (unsigned int j = 0; j < edges.size(); j++) {
            Edge& e = edges[j];
            if (e.nextNode == UINT_MAX) {
                e.nextNode = this->graph.size();
            }
        }
    }
}
