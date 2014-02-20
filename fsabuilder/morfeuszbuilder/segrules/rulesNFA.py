'''
Created on 24 sty 2014

@author: mlenart
'''

from morfeuszbuilder.fsa import fsa, state, encode

class RulesNFAState(object):
    
    statesCounter = 0
    
    def __init__(self, initial=False, final=False, weak=False):
        self.transitionsMap = {}
        self.initial = initial
        self.final = final
        self.weak = weak
        self.idx = RulesNFAState.statesCounter
        RulesNFAState.statesCounter += 1
    
    def addTransition(self, label, targetState):
        self.transitionsMap.setdefault(label, set())
        self.transitionsMap[label].add(targetState)
    
    def getClosure(self, visited):
        if self in visited:
            return set()
        else:
            visited.add(self)
            res = set()
            res.add(self)
            for nextState in self.transitionsMap.get(None, []):
                if self.idx in [6,8,4]:
                    print nextState.idx
                    print self.transitionsMap
                res |= nextState.getClosure(visited)
            return res
    
    def dfs(self, visitedStates=set()):
        if not self in visitedStates:
            visitedStates.add(self)
            yield self
            for _, nextStates in self.transitionsMap.iteritems():
                for state in nextStates:
                    for state1 in state.dfs():
                        yield state1
    
    def debug(self):
        print '----------------'
        print 'STATE:', self.idx
        for label, nextStates in self.transitionsMap.iteritems():
            print label, '-->', [s.idx for s in sorted(nextStates, key=lambda s: s.idx)]

class RulesNFA(object):
    
    def __init__(self):
        self.initialState = RulesNFAState(initial=True)
    
    def _groupOutputByLabels(self, nfaStates):
        res = {}
        for nfaState in nfaStates:
            for label, nextStates in nfaState.transitionsMap.iteritems():
                if label is not None:
                    res.setdefault(label, set())
                    for nextNFAState in nextStates:
                        res[label] |= nextNFAState.getClosure(set())
#                         print 'closure of', nextNFAState.idx, 'is', [s.idx for s in sorted(nextNFAState.getClosure(), key=lambda s: s.idx)]
        return res
    
    def _doConvertState(self, dfaState, nfaStates, nfaSubset2DFAState):
        assert all(map(lambda state: state.weak, nfaStates)) \
            or not any(map(lambda state: state.weak, nfaStates))
        weak = all(map(lambda state: state.weak, nfaStates))
        final = any(map(lambda state: state.final, nfaStates))
        assert not weak or not final
        if final:
            # dfaState should be final
            # and contain info about weakness
            dfaState.encodedData = bytearray([1 if weak else 0])
        for label, nextNFAStates in self._groupOutputByLabels(nfaStates).iteritems():
#             print '============'
#             print 'states:', [s.idx for s in sorted(nfaStates, key=lambda s: s.idx)]
#             print 'label:', label
#             print 'nextStates:', [s.idx for s in sorted(nextNFAStates, key=lambda s: s.idx)]
            key = frozenset(nextNFAStates)
            if key in nfaSubset2DFAState:
                nextDFAState = nfaSubset2DFAState[key]
            else:
                nextDFAState = state.State()
                nfaSubset2DFAState[key] = nextDFAState
                self._doConvertState(nextDFAState, nextNFAStates, nfaSubset2DFAState)
            dfaState.setTransition(label, nextDFAState)
    
    def convertToDFA(self):
        dfa = fsa.FSA(encoder=None, encodeData=False, encodeWords=False)
        startStates = self.initialState.getClosure(set())
        assert not any(filter(lambda s: s.final, startStates))
        dfa.initialState = state.State(additionalData=False)
        self._doConvertState(dfa.initialState, startStates, {frozenset(startStates): dfa.initialState})
        return dfa
    
    def debug(self):
        for state in self.initialState.dfs():
            state.debug()
    