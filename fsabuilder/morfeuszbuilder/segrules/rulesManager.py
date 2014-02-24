'''
Created on 20 lut 2014

@author: mlenart
'''

class RulesManager(object):
    
    def __init__(self):
        self.options2DFA = {}
    
    def _options2Key(self, optionsMap):
        return frozenset(optionsMap.items())
    
    def getDFA(self, optionsMap):
        return self.options2DFA[self._options2Key(optionsMap)]
    
    def addDFA(self, optionsMap, dfa):
        self.options2DFA[self._options2Key(optionsMap)] = dfa
    
    def serialize(self):
        pass