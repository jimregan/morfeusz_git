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

//struct EdgeLabel {
//    int type;
//    const char* textStart;
//    int textLength;
//    
//    bool operator==(const EdgeLabel &el) const {
//        return this->type == el.type 
//                && this->textStart == el.textStart
//                && this->textLength == el.textLength;
//    }
//
//    bool operator<(const coord &o) {
//        return x < o.x || (x == o.x && y < o.y);
//    }
//};

class FlexionGraph {
public:
    
    explicit FlexionGraph(int startNode);
    
    void addPath(const std::vector<InterpretedChunk>& path);
    
    void appendToResults(const Tagset& tagset, std::vector<MorphInterpretation>& results);
    
    bool empty() const;
    
//    virtual ~FlexionGraph();
private:
    
    void addStartEdge(const Edge& e);
    
    void addMiddleEdge(const Edge& e);
    
    void minimizeGraph();
    
    int startNode;
    std::vector< std::vector<Edge> > graph;
};

#endif	/* FLEXIONGRAPH_HPP */

