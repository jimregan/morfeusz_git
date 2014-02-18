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

class TagRule(SegmentRule):
    
    def __init__(self, tagType, line):
        self.tagType = tagType
        self.line = line

class UnaryRule(SegmentRule):
    
    def __init__(self, child, line):
        self.child = child
        self.line = line

class ComplexRule(SegmentRule):
    
    def __init__(self, children, line):
        self.children = children
        self.line = line

class ConcatRule(ComplexRule):
    
    def __init__(self, children, line):
        super(ConcatRule, self).__init__(children, line)
    
class OrRule(ComplexRule):
    
    def __init__(self, children, line):
        super(OrRule, self).__init__(children, line)
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child, line):
        super(ZeroOrMoreRule, self).__init__(child, line)
        
class OneOrMoreRule(UnaryRule):
    
    def __init__(self, child, line):
        super(OneOrMoreRule, self).__init__(child, line)
    
class IgnoreOrthRule(UnaryRule):
    
    def __init__(self, child, line):
        super(IgnoreOrthRule, self).__init__(child, line)
