'''
Created on 20 lut 2014

@author: mlenart
'''
import logging
from morfeuszbuilder.utils.serializationUtils import htons, htonl

class RulesManager(object):
    
    def __init__(self, segtypes):
        self.options2DFA = {}
        self.segtypes = segtypes
    
    def _options2Key(self, optionsMap):
        return frozenset(optionsMap.items())
    
    def _key2Options(self, optionsKey):
        return dict(optionsKey)
    
    def getDFA(self, optionsMap):
        return self.options2DFA[self._options2Key(optionsMap)]
    
    def addDFA(self, optionsMap, dfa):
        self.options2DFA[self._options2Key(optionsMap)] = dfa
    
    def lexeme2SegmentTypeNum(self, lemma, tagnum):
        res = self.segtypes.lexeme2Segnum(lemma, tagnum)
        if res is None:
            raise ValueError()
        else:
            return res
    
    def serialize(self):
        res = bytearray()
        dfasNum = len(self.options2DFA)
        assert dfasNum > 0 and dfasNum < 256
        res.append(dfasNum)
        for key, dfa in self.options2DFA.iteritems():
            optionsMap = self._key2Options(key)
            res.extend(self._serializeOptionsMap(optionsMap))
            res.extend(self._serializeDFA(dfa))
        logging.info('segmentation rules size: %s bytes', len(res))
        return res
    
    def _serializeOptionsMap(self, optionsMap):
        assert len(optionsMap) < 256
        res = bytearray()
        res.extend(self._serializeString(optionsMap['aggl']))
        res.extend(self._serializeString(optionsMap['praet']))
        return res
    
    def _serializeDFA(self, dfa):
        res = bytearray()
#         serializer = SimpleSerializer(dfa, serializeTransitionsData=True)
        dfaBytearray = dfa.serialize()
        res.extend(htonl(len(dfaBytearray)))
        res.extend(dfaBytearray)
        return res
    
    def _serializeString(self, string):
        res = bytearray()
#         res.append(len(string))
        res.extend(string.encode('utf8'))
        res.append(0)
        return res