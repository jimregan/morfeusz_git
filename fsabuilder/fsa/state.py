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
        self.encodedData = None
        self.reverseOffset = None
        self.offset = None
    
    def setTransition(self, byte, nextState):
        self.transitionsMap[byte] = nextState
    
    def hasNext(self, byte):
        return byte in self.transitionsMap
    
    def getNext(self, byte):
        return self.transitionsMap.get(byte, None)
    
    def getRegisterKey(self):
        return ( frozenset(self.transitionsMap.iteritems()), tuple(self.encodedData) if self.encodedData else None )
    
    def isAccepting(self):
        return self.encodedData is not None
    
    def tryToRecognize(self, word):
        if word:
            nextState = self.getNext(word[0])
            if nextState:
                return nextState.tryToRecognize(word[1:])
            else:
                return False
        else:
            return self.encodedData
    
    def dfs(self, alreadyVisited):
        if not self in alreadyVisited:
            for _, state in sorted(self.transitionsMap.iteritems()):
                for state1 in state.dfs(alreadyVisited):
                    yield state1
            alreadyVisited.add(self)
            yield self
