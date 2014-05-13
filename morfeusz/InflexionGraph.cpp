
#include <string>
#include <cassert>
#include <climits>
#include <vector>
#include <iostream>
#include "utils.hpp"
#include "InflexionGraph.hpp"

using namespace std;

void InflexionGraph::addStartEdge(const Edge& e) {
    if (this->graph.empty()) {
        assert(this->node2ChunkStartPtr.empty());
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.textStartPtr);
    }
    assert(this->node2ChunkStartPtr[0] == e.chunk.textStartPtr);
    this->graph[0].push_back(e);
}

void InflexionGraph::addMiddleEdge(unsigned int startNode, const Edge& e) {
    assert(startNode < e.nextNode);
    assert(startNode == this->graph.size());
    if (startNode == this->graph.size()) {
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.textStartPtr);
    }
    this->graph[startNode].push_back(e);
}

static inline bool chunkIsAtFront(
        const InterpretedChunk& chunk, 
        const std::vector<InterpretedChunk>& path) {
    unsigned int i;
    for (i = 0; i < path.size() - 1 && path[i].orthWasShifted; i++) {
    }
    assert(!path[i].orthWasShifted);
    return &chunk == &(path[i]);
}

static inline bool chunkIsAtBack(
        const InterpretedChunk& chunk, 
        const std::vector<InterpretedChunk>& path) {
    return &chunk == &(path.back());
}

static inline bool chunkIsTheOnlyOne(
        const InterpretedChunk& chunk, 
        const std::vector<InterpretedChunk>& path) {
    return chunkIsAtFront(chunk, path) && chunkIsAtBack(chunk, path);
}

void InflexionGraph::addPath(const std::vector<InterpretedChunk>& path, bool weak) {
    //    debugPath(path);
    //    debugGraph(this->graph);
    if (weak && !this->empty() && !this->onlyWeakPaths) {
        return;
    }
    else if (this->onlyWeakPaths && !weak) {
        this->graph.clear();
        this->node2ChunkStartPtr.clear();
        this->onlyWeakPaths = false;
    }
    for (unsigned int i = 0; i < path.size(); i++) {
        const InterpretedChunk& chunk = path[i];
        if (!chunk.orthWasShifted) {
            if (chunkIsTheOnlyOne(chunk, path)) {
                Edge e = {chunk, UINT_MAX};
                this->addStartEdge(e);
            }
            else if (chunkIsAtFront(chunk, path)) {
                Edge e = {chunk, this->graph.empty() ? 1 : (unsigned int) this->graph.size()};
                this->addStartEdge(e);
            }
            else if (chunkIsAtBack(chunk, path)) {
                Edge e = {chunk, UINT_MAX};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            }
            else {
                Edge e = {chunk, (int) this->graph.size() + 1};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            }
        }
    }
}

bool InflexionGraph::canMergeNodes(unsigned int node1, unsigned int node2) {
    return this->node2ChunkStartPtr[node1] == this->node2ChunkStartPtr[node2]
            && this->getPossiblePaths(node1) == this->getPossiblePaths(node2);
}

set<InflexionGraph::Path> InflexionGraph::getPossiblePaths(unsigned int node) {
    if (node == UINT_MAX || node == this->graph.size() - 1) {
        return set<InflexionGraph::Path>();
    } else {
        set<InflexionGraph::Path> res;
        vector<Edge>& edges = this->graph.at(node);
        for (unsigned int i = 0; i < edges.size(); i++) {
            Edge& e = edges[i];
            InflexionGraph::PathElement pathElem(e.chunk.textStartPtr, e.chunk.segmentType);
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

static bool containsEqualEdge(const vector<InflexionGraph::Edge>& edges, const InflexionGraph::Edge& e) {
    for (unsigned int i = 0; i < edges.size(); i++) {
        const InflexionGraph::Edge& e1 = edges[i];
        if (e1.chunk.textStartPtr == e.chunk.textStartPtr
                && e1.chunk.lowercaseCodepoints == e.chunk.lowercaseCodepoints
                && e1.chunk.segmentType == e.chunk.segmentType
                && e1.nextNode == e.nextNode) {
            return true;
        }
    }
    return false;
}

void InflexionGraph::redirectEdges(unsigned int fromNode, unsigned int toNode) {
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

void InflexionGraph::doRemoveNode(unsigned int node) {
    for (unsigned int i = node + 1; i < this->graph.size(); i++) {
        redirectEdges(i, i - 1);
        this->graph[i - 1] = this->graph[i];
        this->node2ChunkStartPtr[i - 1] = this->node2ChunkStartPtr[i];
    }
    this->graph.pop_back();
    this->node2ChunkStartPtr.pop_back();
}

void InflexionGraph::doMergeNodes(unsigned int node1, unsigned int node2) {
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

bool InflexionGraph::tryToMergeTwoNodes() {
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

void InflexionGraph::minimizeGraph() {
    if (this->graph.size() > 2) {
        //        debugGraph(this->graph);
        while (this->tryToMergeTwoNodes()) {
            //            debugGraph(this->graph);
        }
    }
}

bool InflexionGraph::empty() const {
    return this->graph.empty();
}

void InflexionGraph::repairLastNodeNumbers() {
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

void InflexionGraph::clear() {
    graph.clear();
    node2ChunkStartPtr.clear();
}
