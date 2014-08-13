'''
Created on Oct 23, 2013

@author: mlenart
'''
import logging
from common import Interpretation4Analyzer
from morfeuszbuilder.fsa.common import Interpretation4Generator
#from morfeuszbuilder.fsa import externalsort

def _mergeEntries(inputLines, lowercase):
    prevKey = None
    prevInterps = None
    for key, interp in inputLines:
        key = key.lower() if lowercase else key
#         print 'key=', key, 'interp=', interp
        if not key:
            print 'key=', key, 'interp=', interp
            assert key
        if prevKey and prevKey == key:
            prevInterps.append(interp)
        else:
            if prevKey:
                yield (prevKey, frozenset(prevInterps))
            prevKey = key
            prevInterps = [interp]
    if prevInterps:
        yield (prevKey, frozenset(prevInterps))

def parseLine(line):
    splitLine = line.strip().split(u'\t')
    if len(splitLine) == 5:
        orth, base, tag, name, qualifier = splitLine
    elif len(splitLine) == 4:
        orth, base, tag, name = splitLine
        qualifier = ''
    elif len(splitLine) == 3:
        orth, base, tag = splitLine
        name = ''
        qualifier = ''
    else:
        raise ValueError('input line "%s" does not have 3, 4 or 5 tab-separated fields' % line)
    return orth, base, tag, name, qualifier

class PolimorfConverter4Analyzer(object):
    
    def __init__(self, tagset, namesMap, qualifiersMap, encoder, inputEncoding, segmentRulesManager):
        self.tagset = tagset
        self.namesMap = namesMap
        self.qualifiersMap = qualifiersMap
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            if line:
                orth, base, tag, name, qualifier = parseLine(line)
                
                tagnum = self.tagset.getTagnum4Tag(tag)
                namenum = self.namesMap[name]
                typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum)
                qualifiers = qualifier.split('|') if qualifier else frozenset([u''])
                qualsnum = self.qualifiersMap[frozenset(qualifiers)]
                yield '\t'.join((
                            orth.encode(self.inputEncoding),
                            base.encode(self.inputEncoding),
                            str(tagnum),
                            str(namenum),
                            str(typenum), 
                            str(qualsnum)))
                            #~ qualifier.encode(self.inputEncoding)))
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split('\t')[0].decode('utf8')))
#         return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split('\t')[0].decode('utf8')))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
                orth, base, tagnum, namenum, typenum, qualsnum = line.split(u'\t')
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                #~ qualifiers = qualifierStr.split('|') if qualifierStr else []
                qualsnum = int(qualsnum)
                yield (orth, Interpretation4Analyzer(orth, base, tagnum, namenum, typenum, qualsnum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=True)

class PolimorfConverter4Generator(object):
    
    def __init__(self, tagset, namesMap, qualifiersMap, encoder, inputEncoding, segmentRulesManager):
        self.tagset = tagset
        self.namesMap = namesMap
        self.qualifiersMap = qualifiersMap
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            if line:
                orth, base, tag, name, qualifier = parseLine(line)
                if base:
                    homonymId = u''
                    if u':' in base:
                        assumedBase, assumedHomonymId = base.split(u':', 1)
                        if assumedBase != u'' and assumedHomonymId != u'' and assumedHomonymId.isalnum():
                            base, homonymId = assumedBase, assumedHomonymId
                    tagnum = self.tagset.getTagnum4Tag(tag)
                    namenum = self.namesMap[name]
                    qualifiers = qualifier.split('|') if qualifier else frozenset([u''])
                    qualsnum = self.qualifiersMap[frozenset(qualifiers)]
                    typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum)
                    
                    #~ print '\t'.join((
                                #~ orth.encode(self.inputEncoding), 
                                #~ base.encode(self.inputEncoding), 
                                #~ str(tagnum), 
                                #~ str(namenum), 
                                #~ str(typenum),
                                #~ homonymId.encode(self.inputEncoding), 
                                #~ qualifier.encode(self.inputEncoding)))
                    yield '\t'.join((
                                orth.encode(self.inputEncoding), 
                                base.encode(self.inputEncoding), 
                                str(tagnum), 
                                str(namenum), 
                                str(typenum),
                                homonymId.encode(self.inputEncoding), 
                                str(qualsnum)))
                else:
                    logging.warn('Ignoring line: "%s" - contains empty lemma', line.strip())
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: (self.encoder.word2SortKey(line.split('\t')[1].decode('utf8')), line))
    
    def _reallyParseLines(self, inputLines):
        prevLine = None
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line and line != prevLine:
                orth, base, tagnum, namenum, typenum, homonymId, qualsnum = line.split(u'\t')
#                 print orth.encode('utf8'), base.encode('utf8'), homonymId
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                del prevLine
                prevLine = line
                qualsnum = int(qualsnum)
                #~ qualifiers = qualifierStr.split('|') if qualifierStr else ()
                yield (base, Interpretation4Generator(orth, base, tagnum, namenum, typenum, homonymId, qualsnum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=False)
