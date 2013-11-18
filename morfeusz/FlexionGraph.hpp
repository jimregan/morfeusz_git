/* 
 * File:   FlexionGraph.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:03
 */

#ifndef FLEXIONGRAPH_HPP
#define	FLEXIONGRAPH_HPP

#include <vector>
#include "InterpretedChunk.hpp"

struct Edge {
    InterpretedChunk chunk;
    int nextNode;
};

class FlexionGraph {
public:
    
    explicit FlexionGraph(int startNode);
    
    void addPath(const std::vector<InterpretedChunk>& path);
    
    void appendToResults(const Tagset& tagset, std::vector<MorphInterpretation>& results);
    
//    virtual ~FlexionGraph();
private:
    int startNode;
    std::vector< std::vector<Edge> > graph;
};

#endif	/* FLEXIONGRAPH_HPP */

