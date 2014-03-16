'''
Created on Oct 23, 2013

@author: mlenart
'''

import logging
from morfeuszbuilder.utils import serializationUtils

class Encoder(object):
    '''
    classdocs
    '''


    def __init__(self, encoding='utf8'):
        '''
        Constructor
        '''
        self.encoding = encoding
    
    def encodeWord(self, word, lowercase=True):
        assert type(word) == unicode
        res = bytearray(word.lower() if lowercase else word, self.encoding)
        return res
    
    def encodeData(self, data):
        raise NotImplementedError()
#         return bytearray(u'|'.join(data).encode(self.encoding)) + bytearray([0])
    
    def decodeData(self, rawData):
        return NotImplementedError()

    def decodeWord(self, rawWord):
        return unicode(str(rawWord).strip('\x00'), self.encoding)
    
    def word2SortKey(self, word):
        return word.lower().encode(self.encoding)
    
    def _encodeTypeNum(self, typenum):
        assert typenum >= 0 and typenum < 256
        return bytearray([typenum])
    
    def _encodeEncodedForm(self, form, withCasePattern, withPrefix=False):
        res = bytearray()
        assert form.cutLength < 256 and form.cutLength >= 0
        if withPrefix:
            res.extend(self.encodeWord(form.prefixToAdd, lowercase=False))
            res.append(0)
        res.append(form.cutLength)
        res.extend(self.encodeWord(form.suffixToAdd, lowercase=False))
        res.append(0)
        if withCasePattern:
            res.extend(self._encodeCasePattern(form.casePattern))
        return res
    
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
#         logging.info((tagnum & 0xFF00) >> 8)
        assert tagnum < 65536 and tagnum >= 0
        res.append((tagnum & 0xFF00) >> 8)
        res.append(tagnum & 0x00FF)
#         logging.info('%d %s %s' % (tagnum, hex(res[0]), hex(res[1])))
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
    
    def _encodeInterps4Type(self, typenum, interpsList, withCasePattern, withPrefix):
        res = bytearray()
        res.extend(self._encodeTypeNum(typenum))
        
        encodedInterpsList = bytearray()
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            encodedInterpsList.extend(self._encodeEncodedForm(interp.encodedForm, withCasePattern=withCasePattern, withPrefix=withPrefix))
            encodedInterpsList.extend(self._encodeTagNum(interp.tagnum))
            encodedInterpsList.extend(self._encodeNameNum(interp.namenum))
        
        res.extend(serializationUtils.htons(len(encodedInterpsList)))
        res.extend(encodedInterpsList)
        return res
    
    def _doEncodeData(self, interpsList, withCasePattern, withPrefix):
        
        assert type(interpsList) == frozenset
        
        segnum2Interps = self._groupInterpsByType(interpsList)
        
        
        res = bytearray()
        firstByte = len(segnum2Interps)
        assert firstByte < 256
        assert firstByte > 0
        res.append(firstByte)
        
        for typenum, interpsList in segnum2Interps.iteritems():
            res.extend(self._encodeInterps4Type(typenum, interpsList, withCasePattern, withPrefix))
            
        
#         for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
#             encodedInterpsList.extend(self._encodeTypeNum(interp.typenum))
#             encodedInterpsList.extend(self._encodeEncodedForm(interp.encodedForm, withCasePattern=withCasePattern, withPrefix=withPrefix))
#             encodedInterpsList.extend(self._encodeTagNum(interp.tagnum))
#             encodedInterpsList.extend(self._encodeNameNum(interp.namenum))
        del interpsList
#         res.extend(serializationUtils.htons(len(encodedInterpsList)))
#         res.extend(encodedInterpsList)
        return res

class MorphEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(encoding)
        self.LEMMA_ONLY_LOWER = 0
        self.LEMMA_UPPER_PREFIX = 1
        self.LEMMA_MIXED_CASE = 2
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList, withCasePattern=True, withPrefix=False)
#         res = bytearray()
#         firstByte = len(interpsList)
#         assert firstByte < 256
#         assert firstByte > 0
#         res.append(firstByte)
#         assert type(interpsList) == frozenset
#         for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
#             res.extend(self._encodeTypeNum(interp.typenum))
#             res.extend(self._encodeEncodedForm(interp.encodedForm, withCasePattern=True, withPrefix=False))
#             res.extend(self._encodeTagNum(interp.tagnum))
#             res.extend(self._encodeNameNum(interp.namenum))
#         del interpsList
#         return res

class Encoder4Generator(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(Encoder4Generator, self).__init__(encoding)
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList, withCasePattern=False, withPrefix=True)
#         res = bytearray()
#         firstByte = len(interpsList)
#         assert firstByte < 256
#         assert firstByte > 0
#         res.append(firstByte)
#         assert type(interpsList) == frozenset
#         for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
#             res.extend(self._encodeTypeNum(interp.typenum))
#             res.extend(self._encodeEncodedForm(interp.encodedForm, withCasePattern=False, withPrefix=True))
#             res.extend(self._encodeTagNum(interp.tagnum))
#             res.extend(self._encodeNameNum(interp.namenum))
#         return res
#     
#     def decodeData(self, data):
#         
