/* 
 * File:   FlexionGraph.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:03
 */

#ifndef FLEXIONGRAPH_HPP
#define	FLEXIONGRAPH_HPP

#include <vector>
#include <set>
#include <utility>
#include "InterpretedChunk.hpp"

class FlexionGraph {
public:

    struct Edge {
        InterpretedChunk chunk;
        unsigned long nextNode;
    };

    void addPath(const std::vector<InterpretedChunk>& path);

    //    void getResults(const Tagset& tagset, const CharsetConverter& charsetConverter, std::vector<MorphInterpretation>& results);

    std::vector< std::vector<FlexionGraph::Edge> >& getTheGraph() {
        repairLastNodeNumbers();
        minimizeGraph();
        return this->graph;
    }

    bool empty() const;


    //    virtual ~FlexionGraph();
private:

    typedef std::pair<const char*, int> PathElement;
    typedef std::set<PathElement> Path;

    void addStartEdge(const Edge& e);

    void addMiddleEdge(const Edge& e);

    void minimizeGraph();

    bool canMergeNodes(unsigned int node1, unsigned int node2);

    void doMergeNodes(unsigned int node1, unsigned int node2);

    bool tryToMergeTwoNodes();

    std::set<Path> getPossiblePaths(unsigned int node);

    void redirectEdges(unsigned int fromNode, unsigned int toNode);

    void doRemoveNode(unsigned int node);
    
    void repairLastNodeNumbers();

    std::vector< std::vector<Edge> > graph;
    std::vector< const char* > node2ChunkStartPtr;
};

#endif	/* FLEXIONGRAPH_HPP */

