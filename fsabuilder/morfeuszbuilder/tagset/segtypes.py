'''
Created on 17 lut 2014

@author: mlenart
'''
import re

class Segtypes(object):
    
    def __init__(self, tagset, segrulesFile):
        
        self.tagset = tagset
        
        self.segrulesConfigFile = segrulesFile
        
        self.segtype2Segnum = {}
        self.patternsList = []
    
    def readTags(self, lines):
        inTags = False
        for lineNum, line in enumerate(lines, start=1):
            header = self._getHeaderValue(line, lineNum)
            if header == 'tags':
                inTags = True
            elif header:
                inTags = False
            elif inTags:
                segtype, pattern = line.strip().split('\t')
                self._validate(
                               u'Segment type must be a lowercase alphanumeric with optional underscores',
                               lineNum,
                               re.match(r'[a-z_]+', segtype))
                self._validate(
                               u'Pattern must contain only ":", "%", "." and lowercase alphanumeric letters',
                               lineNum,
                               re.match(r'[a-z_\.\:\%]+', pattern))
                
                if segtype in self.segtype2Segnum:
                    segnum = self.segtype2Segnum[segtype]
                else:
                    segnum = len(self.segtype2Segnum)
                    self.segtype2Segnum[segtype] = segnum
                
                self.patternsList.append(SegtypePattern(None, pattern, segnum))
    
    def readLexemes(self, lines):
        inLexemes = False
        for lineNum, line in enumerate(lines, start=1):
            header = self._getHeaderValue(line, lineNum)
            if header == 'lexemes':
                inLexemes = True
            elif header:
                inLexemes = False
            elif inLexemes:
                segtype, pattern = line.strip().split('\t')
                self._validate(
                               u'Segment type must be a lowercase alphanumeric with optional underscores',
                               lineNum,
                               re.match(r'[a-z_]+', segtype))
                self._validate(
                               u'Pattern must contain lemma and POS',
                               lineNum,
                               re.match(r'\w+\:[a-z_]+', pattern, re.U))
                
                if segtype in self.segtype2Segnum:
                    segnum = self.segtype2Segnum[segtype]
                else:
                    segnum = len(self.segtype2Segnum)
                    self.segtype2Segnum[segtype] = segnum
                
                lemma, pos = pattern.split(':')
                
                self.patternsList.append(SegtypePattern(lemma, pos + ':%', segnum))
    
    def lexeme2Segnum(self, lemma, tag):
        for p in self.patternsList:
            res = p.tryToMatch(lemma, tag)
            if res >= 0:
                return res
        raise SegtypesException('Cannot find segment type for given tag: %s' % tag)
    
class SegtypePattern(object):
    
    def __init__(self, lemma, pattern, segnum):
        self.lemma = lemma
        self.pattern = pattern
        self.segnum = segnum
    
    def tryToMatch(self, lemma, tag):
        if (self.lemma is None or self.lemma == lemma) \
        and re.match(self.pattern.replace('%', '.*'), tag):
            return self.segnum
        else:
            return -1

class SegtypesException(Exception):
    
    def __init__(self, msg):
        self.msg = msg
    
    def __str__(self):
        return u'Error in segment rules: %s' % self.msg
