'''
Created on 24 sty 2014

@author: mlenart
'''

import copy
from morfeuszbuilder.segrules.rulesNFA import RulesNFAState

class SegmentRule(object):

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
    
    def transformToGeneratorVersion(self):
        raise NotImplementedError()
    
    def isSinkRule(self):
        return False
    
    def isShiftOrthRule(self):
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
        res = self.segtype
        res += '(' + str(self.segnum) + ')'
        if self.shiftOrth:
            res += '>'
        return res
#         return u'%s(%d)' % (self.segtype, self.segnum)
    
    def transformToGeneratorVersion(self):
        return copy.deepcopy(self)
    
    def isShiftOrthRule(self):
        return self.shiftOrth

class UnaryRule(SegmentRule):
    
    def __init__(self, child, linenum):
        super(UnaryRule, self).__init__(linenum)
        self.child = child
        assert not child.isSinkRule()
    
    def isShiftOrthRule(self):
        return self.child.isShiftOrthRule()

class ComplexRule(SegmentRule):
    
    def __init__(self, children, linenum):
        super(ComplexRule, self).__init__(linenum)
        self.children = children
        assert not any(map(lambda c: c.isSinkRule(), children))
    
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
    
    def isShiftOrthRule(self):
        return all(map(lambda c: c.isShiftOrthRule(), self.children))
    
    def transformToGeneratorVersion(self):
        newChildren = [child.transformToGeneratorVersion() for child in self.children if not child.allowsEmptySequence() or child.isShiftOrthRule()]
        if newChildren == []:
            return SinkRule()
        hasNonOptionalNonShiftingRule = False
        for child in newChildren:
#             print 'child=', child
            if child.isSinkRule() or hasNonOptionalNonShiftingRule:
                return SinkRule()
            elif not child.isShiftOrthRule():
                hasNonOptionalNonShiftingRule = True
#                 print 'got nonshifting'
        return ConcatRule(newChildren, self.linenum)
    
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
    
    def isShiftOrthRule(self):
        return all(map(lambda c: c.isShiftOrthRule(), self.children))
    
    def transformToGeneratorVersion(self):
        newChildren = [child.transformToGeneratorVersion() for child in self.children if not child.allowsEmptySequence() or child.isShiftOrthRule()]
        newChildren = filter(lambda c: not c.isSinkRule(), newChildren)
        if newChildren == []:
            return SinkRule()
        else:
            return OrRule(newChildren, self.linenum)
    
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
    
    def transformToGeneratorVersion(self):
        if self.isShiftOrthRule():
            return copy.deepcopy(self)
        else:
            return SinkRule()
    
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
    
    def transformToGeneratorVersion(self):
        if self.isShiftOrthRule():
            return copy.deepcopy(self)
        else:
            return self.child.transformToGeneratorVersion()
    
    def __str__(self):
        return u'(' + str(self.child) + ')?'

class SinkRule(SegmentRule):
    
    def __init__(self):
        super(SinkRule, self).__init__(None)
    
    def addToNFA(self, fsa):
        return
    
    def allowsEmptySequence(self):
        return False
    
    def _doAddToNFA(self, startStates, endState):
        return
    
    def transformToGeneratorVersion(self):
        return self
    
    def isSinkRule(self):
        return True
    
    def __str__(self):
        return '<<REMOVED>>'
