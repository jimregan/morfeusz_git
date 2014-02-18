'''
Created on 24 sty 2014

@author: mlenart
'''

class SegmentsFSAState(object):
    
    def __init__(self):
        self.transitionsMap = {}
    
    def addSegmentRule(self, segmentRule):
        pass

class SegmentsFSA(object):
    
    def __init__(self, key2Def={}):
        self.initialState = SegmentsFSAState()
    
    def addSegmentRule(self, segmentRule):
        self.initialState.addSegmentRule(segmentRule)
    
    def serialize(self):
        res = bytearray()
        return res
    
    