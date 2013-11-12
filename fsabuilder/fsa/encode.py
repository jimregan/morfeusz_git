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
    
    def word2SortKey(self, word):
        return word.lower().encode(self.encoding)

class SimpleEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(SimpleEncoder, self).__init__(encoding)
    
    def encodeData(self, data):
        return bytearray(data, encoding=self.encoding) + bytearray([0])
    
    def decodeData(self, rawData):
        return unicode(str(rawData)[:-1], self.encoding)

class MorphEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(encoding)
    
    def encodeData(self, interpsList):
        res = bytearray()
#         print interpsList
        firstByte = len(interpsList)
        assert firstByte < 256
        assert firstByte > 0
        res.append(firstByte)
        assert type(interpsList) == frozenset
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            res.extend(self._encodeLemma(interp.lemma))
            res.extend(self._encodeTagNum(interp.tagnum))
            res.extend(self._encodeNameNum(interp.namenum))
        return res
    
    def _encodeLemma(self, lemma):
        res = bytearray()
        assert lemma.cutLength < 256 and lemma.cutLength >= 0
        res.append(lemma.cutLength)
        res.extend(lemma.suffixToAdd)
        res.append(0)
        return res
    
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
    