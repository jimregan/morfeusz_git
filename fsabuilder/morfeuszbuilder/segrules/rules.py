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
        
        self.weak = False
    
    def setWeak(self, weak):
        self.weak = weak
        return self
    
    def addToNFA(self, fsa):
        raise NotImplementedError()
    
    def _doAddToNFA(self, startStates, endState):
        raise NotImplementedError()

class TagRule(SegmentRule):
    
    def __init__(self, segnum, shiftOrth, segtype):
        self.segnum = segnum
        self.segtype = segtype
        self.shiftOrth = shiftOrth
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(final=True, weak=self.weak)
        self._doAddToNFA(fsa.initialState, endState)
    
    def _doAddToNFA(self, startState, endState):
        startState.addTransition((self.segnum, self.shiftOrth), endState)
    
    def __str__(self):
        return u'%s(%d)' % (self.segtype, self.segnum)

class UnaryRule(SegmentRule):
    
    def __init__(self, child):
        self.child = child

class ComplexRule(SegmentRule):
    
    def __init__(self, children):
        self.children = children
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(final=True, weak=self.weak)
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
    
    def __str__(self):
        return u' '.join(map(lambda c: str(c), self.children))
    
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
    
    def __str__(self):
        return u'|'.join(map(lambda c: str(c), self.children))
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child):
        super(ZeroOrMoreRule, self).__init__(child)
        assert isinstance(child, SegmentRule)
    
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
    
    def __str__(self):
        return u'(' + str(self.child) + ')*'
