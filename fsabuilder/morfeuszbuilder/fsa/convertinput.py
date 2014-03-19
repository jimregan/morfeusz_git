'''
Created on Oct 23, 2013

@author: mlenart
'''
import logging
from common import Interpretation4Analyzer
from morfeuszbuilder.fsa.common import Interpretation4Generator

def _mergeEntries(inputLines, lowercase):
    prevKey = None
    prevInterps = None
    for key, interp in inputLines:
        key = key.lower() if lowercase else key
#         print key
        assert key
        if prevKey and prevKey == key:
            prevInterps.append(interp)
        else:
            if prevKey:
                yield (prevKey, frozenset(prevInterps))
            prevKey = key
            prevInterps = [interp]
    yield (prevKey, frozenset(prevInterps))

def _parseLine(line):
    splitLine = line.split(u'\t')
    if len(splitLine) == 4:
        orth, base, tag, name = splitLine
    elif len(splitLine) == 3:
        orth, base, tag = splitLine
        name = ''
    else:
        raise ValueError('input line "%s" does not have 3 or 4 tab-separated fields', line)
    return orth, base, tag, name

class PolimorfConverter4Analyzer(object):
    
    def __init__(self, tagset, encoder, inputEncoding, segmentRulesManager, trimSupneg):
        self.tagset = tagset
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
        self.trimSupneg = trimSupneg
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            orth, base, tag, name = _parseLine(line)
            
            if self.trimSupneg and orth.startswith(u'nie') and tag.endswith(':neg'):
                orth = orth[3:]
            elif self.trimSupneg and orth.startswith(u'naj') and tag.endswith(':sup'):
                orth = orth[3:]
            
            tagnum = self.tagset.getTagnum4Tag(tag)
            namenum = self.tagset.getNamenum4Name(name)
#             typenum = tag2typenum.get(tag, 0)
            typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum)
            yield '%s %s %d %d %d' % (orth.encode(self.inputEncoding), 
                                      base.encode(self.inputEncoding), 
                                      tagnum, namenum, typenum)
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split(' ')[0].decode('utf8')))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
                orth, base, tagnum, namenum, typenum = line.split(u' ')
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                yield (orth, Interpretation4Analyzer(orth, base, tagnum, namenum, typenum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=True)

class PolimorfConverter4Generator(object):
    
    def __init__(self, tagset, encoder, inputEncoding, segmentRulesManager):
        self.tagset = tagset
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            orth, base, tag, name = _parseLine(line)
            if base:
                tagnum = self.tagset.getTagnum4Tag(tag)
                namenum = self.tagset.getNamenum4Name(name)
                typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum)
                yield '%s %s %d %d %d' % (
                                   orth.encode(self.inputEncoding), 
                                   base.encode(self.inputEncoding), 
                                   tagnum, namenum, typenum)
            else:
                logging.warn('Ignoring line: %s', line.strip())
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: (self.encoder.word2SortKey(line.split(' ')[1].decode('utf8')), line))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
                orth, base, tagnum, namenum, typenum = line.split(u' ')
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                yield (base, Interpretation4Generator(orth, base, tagnum, namenum, typenum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=False)
