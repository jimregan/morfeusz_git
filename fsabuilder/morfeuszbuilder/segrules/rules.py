'''
Created on 24 sty 2014

@author: mlenart
'''

from morfeuszbuilder.segrules.rulesNFA import RulesNFAState

class SegmentRule(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
    
    def addToNFA(self, fsa):
        raise NotImplementedError()
    
    def _doAddToNFA(self, startStates, endState):
        raise NotImplementedError()

class TagRule(SegmentRule):
    
    def __init__(self, segnum):
        self.segnum = segnum
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(final=True)
        self._doAddToNFA(fsa.initialState, endState)
    
    def _doAddToNFA(self, startState, endState):
        startState.addTransition(self.segnum, endState)

class UnaryRule(SegmentRule):
    
    def __init__(self, child):
        self.child = child

class ComplexRule(SegmentRule):
    
    def __init__(self, children):
        self.children = children
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(final=True)
        self._doAddToNFA(fsa.initialState, endState)

class ConcatRule(ComplexRule):
    
    def __init__(self, children):
        super(ConcatRule, self).__init__(children)
    
    def _doAddToNFA(self, startState, endState):
        currStartState = startState
        for child in self.children[:-1]:
            currEndState = RulesNFAState()
            child._doAddToNFA(currStartState, currEndState)
            nextStartState = RulesNFAState()
            currEndState.addTransition(None, nextStartState)
            currStartState = nextStartState
        lastChild = self.children[-1]
        lastChild._doAddToNFA(currStartState, endState)
    
class OrRule(ComplexRule):
    
    def __init__(self, children):
        super(OrRule, self).__init__(children)
    
    def _doAddToNFA(self, startState, endState):
        for child in self.children:
            intermStartState = RulesNFAState()
            intermEndState = RulesNFAState()
            startState.addTransition(None, intermStartState)
            child._doAddToNFA(intermStartState, intermEndState)
            intermEndState.addTransition(None, endState)
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child):
        super(ZeroOrMoreRule, self).__init__(child)
    
    def addToNFA(self, fsa):
        raise ValueError()
    
    def _doAddToNFA(self, startState, endState):
        intermStartState = RulesNFAState()
        intermEndState = RulesNFAState()
        
        startState.addTransition(None, intermStartState)
        startState.addTransition(None, endState)
        self.child._doAddToNFA(intermStartState, intermEndState)
        intermEndState.addTransition(None, endState)
        endState.addTransition(None, intermStartState)
    
class IgnoreOrthRule(UnaryRule):
    
    def __init__(self, child):
        super(IgnoreOrthRule, self).__init__(child)
    
    def _doAddToNFA(self, startState, endState):
        startState.addTransition(self.child.segnum, endState, ignoreOrth=True)

