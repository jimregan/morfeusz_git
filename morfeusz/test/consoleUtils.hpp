/* 
 * File:   consoleUtils.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 7:36 PM
 */

#ifndef CONSOLEUTILS_HPP
#define	CONSOLEUTILS_HPP

#include <vector>
#include <string>
#include "morfeusz2.h"

namespace morfeusz {

template <class OutputStream>
void appendMorfeuszResults(const std::vector<MorphInterpretation>& res, OutputStream& out) {
    int prevStart = -1;
    int prevEnd = -1;
    out << "[";
    for (unsigned int i = 0; i < res.size(); i++) {
        const MorphInterpretation& mi = res[i];
        if (prevStart != -1
                && (prevStart != mi.getStartNode() || prevEnd != mi.getEndNode())) {
            out << "]\n[";
        }
        else if (prevStart != -1) {
            out << "; ";
        }
        out << mi.getStartNode() << ","
                << mi.getEndNode() << ","
                << mi.getOrth() << ","
                << mi.getLemma() << ","
                << mi.getTag() << ","
                << mi.getName();
        prevStart = mi.getStartNode();
        prevEnd = mi.getEndNode();
    }
    out << "]\n";
}

}

#endif	/* CONSOLEUTILS_HPP */

