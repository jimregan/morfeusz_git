'''
Created on 24 sty 2014

@author: mlenart
'''

from morfeuszbuilder.segrules.rulesFSA import RulesFSA, RulesState

class RulesNFAState(object):
    
    statesCounter = 0
    
    def __init__(self, initial=False, final=False, weak=False):
        self.transitionsMap = {}
#         self.transitionsDataMap = {}
        self.initial = initial
        self.final = final
        self.weak = weak
        self.idx = RulesNFAState.statesCounter
        RulesNFAState.statesCounter += 1
    
    def addTransition(self, label, targetState):
        assert label is None or len(label) == 2
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
                res |= nextState.getClosure(visited)
            return res
    
    def dfs(self, visitedStates=set()):
        if not self in visitedStates:
            visitedStates.add(self)
            yield self
            for _, nextStates in self.transitionsMap.iteritems():
                for state in nextStates:
                    for state1 in state.dfs(visitedStates):
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
#                     transitionData = nfaState.transitionsDataMap[label]
                    segnum, shiftOrth = label
                    res.setdefault((segnum, shiftOrth), set())
                    for nextNFAState in nextStates:
                        res[(segnum, shiftOrth)] |= nextNFAState.getClosure(set())
        return res
    
    def _doConvertState(self, dfaState, nfaStates, nfaSubset2DFAState):
        assert all(map(lambda state: state.weak, nfaStates)) \
            or not any(map(lambda state: state.weak, nfaStates))
        weak = all(map(lambda state: state.weak or not state.final, nfaStates))
        final = any(map(lambda state: state.final, nfaStates))
        assert not weak or not final
        if final:
            # dfaState should be final
            # and contain info about weakness
            dfaState.setAsAccepting(weak=weak)
#             dfaState.encodedData = bytearray([1 if weak else 0])
        for (segnum, shiftOrth), nextNFAStates in self._groupOutputByLabels(nfaStates).iteritems():
            key = frozenset(nextNFAStates)
            if key in nfaSubset2DFAState:
                nextDFAState = nfaSubset2DFAState[key]
            else:
                nextDFAState = RulesState()
                nfaSubset2DFAState[key] = nextDFAState
                self._doConvertState(nextDFAState, nextNFAStates, nfaSubset2DFAState)
            dfaState.setTransition((segnum, shiftOrth), nextDFAState)
#             dfaState.setTransitionData(label, transitionData)
    
    def convertToDFA(self):
        dfa = RulesFSA()
        startStates = self.initialState.getClosure(set())
        assert not any(filter(lambda s: s.final, startStates))
        dfa.initialState = RulesState()
        self._doConvertState(dfa.initialState, startStates, {frozenset(startStates): dfa.initialState})
        return dfa
    
    def debug(self):
        for state in self.initialState.dfs():
            state.debug()
    