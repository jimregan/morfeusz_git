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


    def __init__(self, encoder):
        self.encodeWord = encoder.encodeWord
        self.encodeData = encoder.encodeData
        self.decodeData = encoder.decodeData
        self.encodedPrevWord = None
        self.initialState = state.State()
        self.register = register.Register()
        self.label2Freq = {0: float('inf')}
    
    def tryToRecognize(self, word, addFreq=False):
        return self.decodeData(self.initialState.tryToRecognize(self.encodeWord(word), addFreq))
    
    def feed(self, input):
        
        allWords = []
        for n, (word, data) in enumerate(input, start=1):
            assert data is not None
            if type(data) in [str, unicode]:
                data = [data]
            encodedWord = self.encodeWord(word)
            assert encodedWord > self.encodedPrevWord
            self._addSorted(encodedWord, self.encodeData(data))
            self.encodedPrevWord = encodedWord
            assert self.tryToRecognize(word) == data
            if n % 10000 == 0:
                logging.info(word)
            allWords.append(word)
            for label in encodedWord:
                self.label2Freq[label] = self.label2Freq.get(label, 0) + 1
        
        self.initialState = self._replaceOrRegister(self.initialState, self.encodeWord(word))
        self.encodedPrevWord = None
        
#         for w in allWords:
#             self.tryToRecognize(w, True)
    
    def train(self, trainData):
        self.label2Freq = {0: float('inf')}
        for word in trainData:
            self.tryToRecognize(word, addFreq=True)
            for label in self.encodeWord(word):
                self.label2Freq[label] = self.label2Freq.get(label, 0) + 1
    
    def getStatesNum(self):
        return self.register.getStatesNum()
        
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
        
        
        