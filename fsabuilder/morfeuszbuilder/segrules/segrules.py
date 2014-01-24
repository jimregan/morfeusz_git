'''
Created on 24 sty 2014

@author: mlenart
'''

class SegmentRule(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''

class SimpleRule(SegmentRule):
    
    def __init__(self, name, typeId):
        self.name = name
        self.identifier = typeId

class ComplexRule(SegmentRule):
    
    def __init__(self, children):
        self.children = children

class ConcatRule(ComplexRule):
    
    def __init__(self, children):
        super(ConcatRule, self).__init__(children)
    
class OrRule(ComplexRule):
    
    def __init__(self, children):
        super(OrRule, self).__init__(children)

class UnaryRule(SegmentRule):
    
    def __init__(self, child):
        self.child = child
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child):
        super(ZeroOrMoreRule, self).__init__(child)
    
class IgnoreOrthRule(UnaryRule):
    
    def __init__(self, child):
        super(IgnoreOrthRule, self).__init__(child)
