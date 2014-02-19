'''
Created on 24 sty 2014

@author: mlenart
'''

from morfeuszbuilder.fsa import fsa, state, encode

class RulesNFAState(object):
    
    def __init__(self, initial=False, final=False):
        self.transitionsMap = {}
        self.initial = initial
        self.final = final
    
    def addTransition(self, label, targetState, ignoreOrth=False):
        assert not ignoreOrth or label is not None
        self.transitionsMap.setdefault((label, ignoreOrth), set())
        self.transitionsMap[(label, ignoreOrth)].add(targetState)

class RulesNFA(object):
    
    def __init__(self, key2Def={}):
        self.initialState = RulesNFAState(initial=True)
    
    def _doConvertState(self, dfaState, nfaStates):
        for label, (nextIgnoreOrth, nextNFAStates) in self._groupOutputByLabels(nfaStates).iteritems():
            nextDFAState = state.State(additionalData=nextIgnoreOrth)
            dfaState.setTransition(label, nextDFAState)
            dfaState.encodedData = bytearray()
            self._doConvertState(nextDFAState, nextNFAStates)
    
    def convertToDFA(self):
        dfa = fsa.FSA(encoder=None, encodeWords=False)
        startStates = self.initialState.getClosure()
        assert not any(filter(lambda s: s.final, startStates))
        dfa.initialState = state.State(additionalData=False)
        self._doConvertState(dfa.initialState, startStates)
                
    