'''
Created on Oct 23, 2013

@author: mlenart
'''

import logging
import itertools
from morfeuszbuilder.utils.serializationUtils import *

class Encoder(object):
    '''
    classdocs
    '''


    def __init__(self, lowercase, encoding='utf8'):
        '''
        Constructor
        '''
        self.lowercase = lowercase
        self.encoding = encoding
        self.qualifiersMap = { frozenset(): 0}
    
    def encodeWord(self, word, lowercase=True):
        assert type(word) == unicode
        res = bytearray(word.lower() if self.lowercase and lowercase else word, self.encoding)
        return res
    
    def encodeData(self, data):
        raise NotImplementedError()
    
    def decodeData(self, rawData):
        return NotImplementedError()

    def decodeWord(self, rawWord):
        return unicode(str(rawWord).strip('\x00'), self.encoding)
    
    def word2SortKey(self, word):
        normalizedWord = word.lower() if self.lowercase else word
        return normalizedWord.encode(self.encoding)
    
    def _encodeTypeNum(self, typenum):
        assert typenum >= 0 and typenum < 256
        return bytearray([typenum])
    
    def _encodeCasePattern(self, casePattern):
        res = bytearray()
        if True not in casePattern:
            res.append(self.LEMMA_ONLY_LOWER)
            return res
        elif self._hasUpperPrefix(casePattern):
            res.append(self.LEMMA_UPPER_PREFIX)
            res.append(self._getUpperPrefixLength(casePattern))
            return res
        else:
            assert len(casePattern) < 256
            res.append(self.LEMMA_MIXED_CASE)
            res.append(len([c for c in casePattern if c]))
            for idx in range(len(casePattern)):
                if casePattern[idx]:
                    res.append(idx)
            return res
    
    def _encodeQualifiers(self, qualifiers):
        res = bytearray()
        key = frozenset(qualifiers)
        if key in self.qualifiersMap:
            n = self.qualifiersMap[key]
        else:
            n = len(self.qualifiersMap)
            self.qualifiersMap[key] = n
        assert n < 500
        res.extend(htons(n))
        return res
    
    def _hasUpperPrefix(self, casePattern):
        for i in range(len(casePattern) + 1):
            if all(casePattern[:i]) and not any(casePattern[i:]):
                return True
        return False
    
    def _getUpperPrefixLength(self, casePattern):
        assert self._hasUpperPrefix(casePattern)
        for i in range(len(casePattern)):
            if not casePattern[i]:
                return i
        return len(casePattern)
    
    def _encodeTagNum(self, tagnum):
        res = bytearray()
        assert tagnum < 65536 and tagnum >= 0
        res.append((tagnum & 0xFF00) >> 8)
        res.append(tagnum & 0x00FF)
        return res
    
    def _encodeNameNum(self, namenum):
        assert namenum < 256 and namenum >= 0
        return bytearray([namenum])
    
    def _groupInterpsByType(self, interpsList):
        res = {}
        for interp in interpsList:
            res.setdefault(interp.typenum, [])
            res[interp.typenum].append(interp)
        return res
    
    def _getOrthCasePatterns(self, interpsList):
        res = []
        for interp in interpsList:
            if not True in interp.orthCasePattern:
                return []
            else:
                res.append(list(interp.orthCasePattern))
        return res
    
    def _encodeInterps4Type(self, typenum, interpsList, isAnalyzer):
        res = bytearray()
        res.extend(self._encodeTypeNum(typenum))
        encodedInterpsList = bytearray()
        if isAnalyzer:
            casePatterns = self._getOrthCasePatterns(interpsList)
            encodedInterpsList.append(len(casePatterns))
            for casePattern in casePatterns:
                encodedInterpsList.extend(self._encodeCasePattern(casePattern))
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            if isAnalyzer:
                encodedInterpsList.extend(self._encodeCasePattern(interp.orthCasePattern))
            else:
                encodedInterpsList.extend(serializeString(interp.homonymId))
                encodedInterpsList.extend(serializeString(interp.encodedForm.prefixToAdd))
            encodedInterpsList.append(interp.encodedForm.cutLength)
            encodedInterpsList.extend(serializeString(interp.encodedForm.suffixToAdd))
            if isAnalyzer:
                encodedInterpsList.extend(self._encodeCasePattern(interp.encodedForm.casePattern))
            encodedInterpsList.extend(htons(interp.tagnum))
            encodedInterpsList.append(interp.namenum)
            encodedInterpsList.extend(self._encodeQualifiers(interp.qualifiers))
        
        res.extend(htons(len(encodedInterpsList)))
        res.extend(encodedInterpsList)
        return res
    
    def _doEncodeData(self, interpsList, isAnalyzer):
        
        assert type(interpsList) == frozenset
        
        segnum2Interps = self._groupInterpsByType(interpsList)
        
        
        res = bytearray()
        firstByte = len(segnum2Interps)
        assert firstByte < 256
        assert firstByte > 0
        res.append(firstByte)
        
        for typenum, interpsList in segnum2Interps.iteritems():
            res.extend(self._encodeInterps4Type(typenum, interpsList, isAnalyzer))
        del interpsList
        
        return res

class MorphEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(True, encoding)
        self.LEMMA_ONLY_LOWER = 0
        self.LEMMA_UPPER_PREFIX = 1
        self.LEMMA_MIXED_CASE = 2
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList, isAnalyzer=True)

class Encoder4Generator(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(Encoder4Generator, self).__init__(False, encoding)
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList, isAnalyzer=False)
