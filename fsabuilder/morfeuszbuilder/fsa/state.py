'''
Created on Oct 8, 2013

@author: mlenart
'''

class State(object):
    '''
    A state in an automaton
    '''
    
    statesCounter = 0

    def __init__(self, additionalData=None):
        self.transitionsMap = {}
        self.freq = 0
        self.encodedData = None
        self.reverseOffset = None
        self.offset = None
        self.label2Freq = {}
        self.serializeAsArray = False
        self.additionalData = additionalData
        
        self.idx = State.statesCounter
        State.statesCounter += 1
    
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
    
    def dfs(self, alreadyVisited, sortKey=lambda (_, state): -state.freq):
        if not self in alreadyVisited:
            alreadyVisited.add(self)
            for _, state in sorted(self.transitionsMap.iteritems(), key=sortKey):
                for state1 in state.dfs(alreadyVisited):
                    yield state1
            yield self
    
    def debug(self):
        print '----------------'
        print 'STATE:', self.idx
        for label, s in self.transitionsMap.iteritems():
            print label, '-->', s.idx
