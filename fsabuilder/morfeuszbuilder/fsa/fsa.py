'''
Created on Oct 8, 2013

@author: mlenart
'''

import state
import register
import logging

class FSA(object):
    '''
    A finite state automaton
    '''


    def __init__(self, encoder, tagset=None, encodeData=True):
        self.encodeWord = encoder.encodeWord
        self.encodeData = encoder.encodeData if encodeData else lambda x: x
        self.decodeData = encoder.decodeData if encodeData else lambda x: x
        self.encodedPrevWord = None
        self.tagset = tagset
        self.initialState = state.State()
        self.register = register.Register()
        self.label2Freq = {}
        self.n = 0
        self.closed = False
    
    def tryToRecognize(self, word, addFreq=False):
        return self.decodeData(self.initialState.tryToRecognize(self.encodeWord(word), addFreq))
    
    def addEntry(self, word, data):
        assert not self.closed
        assert data is not None
        encodedWord = self.encodeWord(word)
        assert encodedWord > self.encodedPrevWord
        self._addSorted(encodedWord, self.encodeData(data))
        self.encodedPrevWord = encodedWord
        
        self.n += 1
        
        # debug
        if self.n % 100000 == 0:
            logging.info(word)
            logging.info(str(self.register.getStatesNum()))
    #             allWords.append(word)
        for label in encodedWord:
            self.label2Freq[label] = self.label2Freq.get(label, 0) + 1
    
    def close(self):
        assert self.n > 0
        assert not self.closed
        self.initialState = self._replaceOrRegister(self.initialState, self.encodedPrevWord)
        self.encodedPrevWord = None
        self.closed = True
    
#     def feed(self, input):
#         
# #         allWords = []
#         for n, (word, data) in enumerate(input, start=1):
#             assert data is not None
#             encodedWord = self.encodeWord(word)
#             assert encodedWord > self.encodedPrevWord
#             if encodedWord > self.encodedPrevWord:
#                 self._addSorted(encodedWord, self.encodeData(data))
#                 self.encodedPrevWord = encodedWord
# #                 assert self.tryToRecognize(word) == data
#                 if n % 10000 == 0:
#                     logging.info(word)
#                     logging.info(str(self.register.getStatesNum()))
#     #             allWords.append(word)
#                 for label in encodedWord:
#                     self.label2Freq[label] = self.label2Freq.get(label, 0) + 1
#         
#         self.initialState = self._replaceOrRegister(self.initialState, self.encodeWord(word))
#         self.encodedPrevWord = None
        
#         for w in allWords:
#             self.tryToRecognize(w, True)
    
    def train(self, trainData):
        self.label2Freq = {}
        for idx, word in enumerate(trainData):
            self.tryToRecognize(word, addFreq=True)
            for label in self.encodeWord(word):
                self.label2Freq[label] = self.label2Freq.get(label, 0) + 1
            if idx % 100000 == 0:
                logging.info(str(idx))
    
    def dfs(self):
        for state in self.initialState.dfs(set()):
            yield state
    
    def getStatesNum(self):
        return self.register.getStatesNum()
    
    def getTransitionsNum(self):
        res = 0
        for s in self.initialState.dfs(set()):
            res += len(s.transitionsMap)
        return res
        
    def _addSorted(self, encodedWord, data):
        assert self.encodedPrevWord < encodedWord
        q = self.initialState
        i = 0
        while i <= len(encodedWord) and q.hasNext(encodedWord[i]):
            q = q.getNext(encodedWord[i])
            i += 1
        if self.encodedPrevWord and i < len(self.encodedPrevWord):
            nextState = q.getNext(self.encodedPrevWord[i])
            q.setTransition(
                                self.encodedPrevWord[i], 
                                self._replaceOrRegister(nextState, self.encodedPrevWord[i+1:]))
        
        while i < len(encodedWord):
            q.setTransition(encodedWord[i], state.State())
            q = q.getNext(encodedWord[i])
            i += 1
        
        assert q.encodedData is None
#         print q, encodedData
        q.encodedData = data
    
    def _replaceOrRegister(self, q, encodedWord):
        if encodedWord:
            nextState = q.getNext(encodedWord[0])
            q.setTransition(
                                encodedWord[0], 
                                self._replaceOrRegister(nextState, encodedWord[1:]))
        
        if self.register.containsEquivalentState(q):
            return self.register.getEquivalentState(q)
        else:
            self.register.addState(q)
            return q
    
    def calculateOffsets(self, sizeCounter):
        currReverseOffset = 0
        for state in self.initialState.dfs(set()):
            currReverseOffset += sizeCounter(state)
            state.reverseOffset = currReverseOffset
        for state in self.initialState.dfs(set()):
            state.offset = currReverseOffset - state.reverseOffset
        