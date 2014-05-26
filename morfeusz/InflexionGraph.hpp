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

class InflexionGraph {
public:
    
    InflexionGraph(): graph(), node2ChunkStartPtr(), onlyWeakPaths(true) {
        
    }

    struct Edge {
        InterpretedChunk chunk;
        unsigned int nextNode;
    };

    void addPath(const std::vector<InterpretedChunk>& path, bool weak);

    //    void getResults(const Tagset& tagset, const CharsetConverter& charsetConverter, std::vector<MorphInterpretation>& results);

    const std::vector< std::vector<InflexionGraph::Edge> >& getTheGraph();

    bool empty() const;
    
    void clear();


    //    virtual ~FlexionGraph();
private:

    typedef std::pair<const char*, int> PathElement;
    typedef std::set<PathElement> Path;

    void addStartEdge(const Edge& e);

    void addMiddleEdge(unsigned int startNode, const Edge& e);

    void minimizeGraph();

    bool canMergeNodes(unsigned int node1, unsigned int node2);

    void doMergeNodes(unsigned int node1, unsigned int node2);

    bool tryToMergeTwoNodes();

    std::set<Path> getPossiblePaths(unsigned int node);

    void redirectEdges(unsigned int fromNode, unsigned int toNode);

    void doRemoveNode(unsigned int node);
    
    void repairLastNodeNumbers();
    
    void repairOrthShifts();

    std::vector< std::vector<Edge> > graph;
    std::vector< const char* > node2ChunkStartPtr;
    bool onlyWeakPaths;
};

#endif	/* FLEXIONGRAPH_HPP */

