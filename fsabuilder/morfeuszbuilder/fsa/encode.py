'''
Created on Oct 23, 2013

@author: mlenart
'''

import logging

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
#         print unicode(str(rawData), self.encoding)[:-1]
#         print unicode(str(rawData), self.encoding)[:-1].split(u'|')
#         return unicode(str(rawData), self.encoding)[:-1].split(u'|')

    def decodeWord(self, rawWord):
        return unicode(str(rawWord).strip('\x00'), self.encoding)
    
    def word2SortKey(self, word):
        return word.lower().encode(self.encoding)
    
    def _encodeTypeNum(self, typenum):
        assert typenum >= 0 and typenum < 256
        return bytearray([typenum])
    
    def _encodeEncodedForm(self, form):
        res = bytearray()
        assert form.cutLength < 256 and form.cutLength >= 0
        res.append(form.cutLength)
        res.extend(self.encodeWord(form.suffixToAdd, lowercase=False))
        res.append(0)
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

# class SimpleEncoder(Encoder):
#     
#     def __init__(self, encoding='utf8'):
#         super(SimpleEncoder, self).__init__(encoding)
#     
#     def encodeData(self, data):
#         return bytearray(data, encoding=self.encoding) + bytearray([0])
#     
#     def decodeData(self, rawData):
#         return unicode(str(rawData)[:-1], self.encoding)

class MorphEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(encoding)
        self.LEMMA_ONLY_LOWER = 0
        self.LEMMA_UPPER_PREFIX = 1
        self.LEMMA_MIXED_CASE = 2
    
    def encodeData(self, interpsList):
        res = bytearray()
#         print interpsList
        firstByte = len(interpsList)
        assert firstByte < 256
        assert firstByte > 0
        res.append(firstByte)
        assert type(interpsList) == frozenset
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            res.extend(self._encodeTypeNum(interp.typenum))
            res.extend(self._encodeEncodedForm(interp.lemma))
            res.extend(self._encodeTagNum(interp.tagnum))
            res.extend(self._encodeNameNum(interp.namenum))
        return res

class Encoder4Generator(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(encoding)
        self.LEMMA_ONLY_LOWER = 0
        self.LEMMA_UPPER_PREFIX = 1
        self.LEMMA_MIXED_CASE = 2
    
    def encodeData(self, interpsList):
        res = bytearray()
#         print interpsList
        firstByte = len(interpsList)
        assert firstByte < 256
        assert firstByte > 0
        res.append(firstByte)
        assert type(interpsList) == frozenset
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            res.extend(self._encodeEncodedForm(interp.orth))
            res.extend(self._encodeTagNum(interp.tagnum))
            res.extend(self._encodeNameNum(interp.namenum))
        return res
    
    def _encodeTypeNum(self, typenum):
        assert typenum >= 0 and typenum < 256
        return bytearray([typenum])
    
    def _encodeLemma(self, lemma):
        res = bytearray()
        assert lemma.cutLength < 256 and lemma.cutLength >= 0
        res.append(lemma.cutLength)
        res.extend(self.encodeWord(lemma.suffixToAdd, lowercase=False))
        res.append(0)
        res.extend(self._encodeCasePattern(lemma.casePattern))
        return res
