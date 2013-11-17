/* 
 * File:   Morfeusz_impl.hpp
 * Author: lennyn
 *
 * Created on November 15, 2013, 1:43 PM
 */

#ifndef MORFEUSZ_IMPL_HPP
#define	MORFEUSZ_IMPL_HPP

#include <cassert>
#include "Morfeusz.hpp"

//template <class OutputIterator>
//void Morfeusz::processOneWord(const char*& inputData, const char* inputEnd, int startNodeNum, OutputIterator output, bool insertIgn = true) const {
//    if (inputData == inputEnd) {
//        return;
//    }
//    const char* start = inputData;
//    StateType state = fsa->getInitialState();
//    int currNodeNum = startNodeNum;
//    do {
//        int codepoint = this->charsetConverter->next(inputData, inputEnd);
//        if (!isSpace(codepoint) && codepoint != 0) {
//            feedAutomaton(state, codepoint);
//            if (state.isAccepting()) {
//                int currInput = inputData;
//                vector<MorphInterpretation> additionalInterps;
//                processOneWord(
//                        currInput, inputEnd, 
//                        currNodeNum + 1, 
//                        back_inserter(additionalInterps), false);
//                if (!additionalInterps.empty()) {
//                    currNodeNum = additionalInterps.back().getEndNode();
//                }
//            }
//        }
//    }
//}

#endif	/* MORFEUSZ_IMPL_HPP */

