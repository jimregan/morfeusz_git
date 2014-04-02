'''
Created on 24 sty 2014

@author: mlenart
'''

from morfeuszbuilder.segrules.rulesNFA import RulesNFAState

class SegmentRule(object):
    '''
    classdocs
    '''


    def __init__(self, linenum):
        
        self.weak = False
        self.linenum = linenum
    
    def setWeak(self, weak):
        self.weak = weak
        return self
    
    def addToNFA(self, fsa):
        raise NotImplementedError()
    
    def allowsEmptySequence(self):
        raise NotImplementedError()
    
    def _doAddToNFA(self, startStates, endState):
        raise NotImplementedError()

class TagRule(SegmentRule):
    
    def __init__(self, segnum, shiftOrth, segtype, linenum):
        self.segnum = segnum
        self.segtype = segtype
        self.shiftOrth = shiftOrth
        self.linenum = linenum
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(self, final=True, weak=self.weak)
        self._doAddToNFA(fsa.initialState, endState)
    
    def _doAddToNFA(self, startState, endState):
        startState.addTransition((self.segnum, self.shiftOrth), endState)
    
    def allowsEmptySequence(self):
        return False
    
    def __str__(self):
        return u'%s(%d)' % (self.segtype, self.segnum)

class UnaryRule(SegmentRule):
    
    def __init__(self, child, linenum):
        self.child = child
        self.linenum = linenum

class ComplexRule(SegmentRule):
    
    def __init__(self, children, linenum):
        self.children = children
        self.linenum = linenum
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(self, final=True, weak=self.weak)
        self._doAddToNFA(fsa.initialState, endState)

class ConcatRule(ComplexRule):
    
    def __init__(self, children, linenum):
        super(ConcatRule, self).__init__(children, linenum)
    
    def _doAddToNFA(self, startState, endState):
        currStartState = startState
        for child in self.children[:-1]:
            currEndState = RulesNFAState(self)
            child._doAddToNFA(currStartState, currEndState)
            nextStartState = RulesNFAState(self)
            currEndState.addTransition(None, nextStartState)
            currStartState = nextStartState
        lastChild = self.children[-1]
        lastChild._doAddToNFA(currStartState, endState)
    
    def allowsEmptySequence(self):
        return all(map(lambda rule: rule.allowsEmptySequence(), self.children))
    
    def __str__(self):
        return u' '.join(map(lambda c: str(c), self.children))
    
class OrRule(ComplexRule):
    
    def __init__(self, children, linenum):
        super(OrRule, self).__init__(children, linenum)
    
    def _doAddToNFA(self, startState, endState):
        for child in self.children:
            intermStartState = RulesNFAState(self)
            intermEndState = RulesNFAState(self)
            startState.addTransition(None, intermStartState)
            child._doAddToNFA(intermStartState, intermEndState)
            intermEndState.addTransition(None, endState)
    
    def allowsEmptySequence(self):
        return any(map(lambda rule: rule.allowsEmptySequence(), self.children))
    
    def __str__(self):
        return u'|'.join(map(lambda c: str(c), self.children))
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child, linenum):
        super(ZeroOrMoreRule, self).__init__(child, linenum)
        assert isinstance(child, SegmentRule)
    
    def addToNFA(self, fsa):
        raise ValueError()
    
    def _doAddToNFA(self, startState, endState):
        intermStartState = RulesNFAState(self)
        intermEndState = RulesNFAState(self)
        
        startState.addTransition(None, intermStartState)
        startState.addTransition(None, endState)
        self.child._doAddToNFA(intermStartState, intermEndState)
        intermEndState.addTransition(None, endState)
        intermEndState.addTransition(None, intermStartState)
    
    def allowsEmptySequence(self):
        return True
    
    def __str__(self):
        return u'(' + str(self.child) + ')*'

class OptionalRule(UnaryRule):
    
    def __init__(self, child, linenum):
        super(OptionalRule, self).__init__(child, linenum)
        assert isinstance(child, SegmentRule)
    
    def addToNFA(self, fsa):
        raise ValueError()
    
    def _doAddToNFA(self, startState, endState):
        intermStartState = RulesNFAState(self)
        intermEndState = RulesNFAState(self)
        
        startState.addTransition(None, intermStartState)
        startState.addTransition(None, endState)
        self.child._doAddToNFA(intermStartState, intermEndState)
        intermEndState.addTransition(None, endState)
    
    def allowsEmptySequence(self):
        return True
    
    def __str__(self):
        return u'(' + str(self.child) + ')?'
    
