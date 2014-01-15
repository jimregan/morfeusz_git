'''
Created on Oct 8, 2013

@author: mlenart
'''

class State(object):
    '''
    A state in an automaton
    '''

    def __init__(self):
        self.transitionsMap = {}
        self.freq = 0
        self.encodedData = None
        self.reverseOffset = None
        self.offset = None
        self.label2Freq = {}
        self.serializeAsArray = False
    
    @property
    def transitionsNum(self):
        return len(self.transitionsMap)
    
    def setTransition(self, byte, nextState):
        self.transitionsMap[byte] = nextState
    
    def hasNext(self, byte):
        return byte in self.transitionsMap
    
    def getNext(self, byte, addFreq=False):
        if addFreq:
            self.freq += 1
            self.label2Freq[byte] = self.label2Freq.get(byte, 0) + 1
        return self.transitionsMap.get(byte, None)
    
    def getRegisterKey(self):
        return ( frozenset(self.transitionsMap.iteritems()), tuple(self.encodedData) if self.encodedData else None )
    
    def isAccepting(self):
        return self.encodedData is not None
    
    def tryToRecognize(self, word, addFreq=False):
        if word:
            nextState = self.getNext(word[0], addFreq)
            if nextState:
                return nextState.tryToRecognize(word[1:], addFreq)
            else:
                return False
        else:
            return self.encodedData
    
    def dfs(self, alreadyVisited=set(), sortKey=lambda (_, state): -state.freq):
        if not self in alreadyVisited:
            for _, state in sorted(self.transitionsMap.iteritems(), key=sortKey):
                for state1 in state.dfs(alreadyVisited):
                    yield state1
            alreadyVisited.add(self)
            yield self
