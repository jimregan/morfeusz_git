'''
Created on 17 lut 2014

@author: mlenart
'''
import re
import logging
from morfeuszbuilder.utils import exceptions

class Segtypes(object):
    
    def __init__(self, tagset, segrulesConfigFile):
        
        self.tagset = tagset
        
        self.filename = segrulesConfigFile.filename
        
        self.segtype2Segnum = {}
        self.segnum2Segtype = {}
        self.patternsList = []
        
        self._tagnum2Segnum = {}
        self._lemmaTagnum2Segnum = {}
        
        self._readLexemes(segrulesConfigFile)
        self._readTags(segrulesConfigFile)
        self._indexSegnums()
        
    def _validate(self, msg, lineNum, cond):
        if not cond:
            raise exceptions.ConfigFileException(self.filename, lineNum, msg)
    
    def _readTags(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('tags'):
            splitLine = re.split(r'\s+', line.strip())
            self._validate(
                           u'Line in [tags] section must contain exactly two fields - segment type and tag pattern', 
                           lineNum,
                           len(splitLine) == 2)
            segtype, pattern = splitLine
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
        
        self.segnum2Segtype = dict([(v, k) for (k, v) in self.segtype2Segnum.iteritems()])
    
    def _readLexemes(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('lexemes'):
            segtype, pattern = line.strip().split('\t')
            self._validate(
                           u'Segment type must be a lowercase alphanumeric with optional underscores',
                           lineNum,
                           re.match(r'[a-z_]+', segtype))
            self._validate(
                           u'Pattern must contain lemma and POS',
                           lineNum,
                           re.match(r'.+\:[a-z_]+', pattern, re.U))
            
            if segtype in self.segtype2Segnum:
                segnum = self.segtype2Segnum[segtype]
            else:
                segnum = len(self.segtype2Segnum)
                self.segtype2Segnum[segtype] = segnum
            
            lemma, pos = pattern.split(':')
            
            self.patternsList.append(SegtypePattern(lemma, pos + ':%', segnum))
    
    def _debugSegnums(self):
        for tagnum, segnum in self._tagnum2Segnum.items():
            print self.tagset.getTag4Tagnum(tagnum), '-->', self.segnum2Segtype[segnum]
        
        for (base, tagnum), segnum in self._lemmaTagnum2Segnum.items():
            print base, self.tagset.getTag4Tagnum(tagnum), '-->', self.segnum2Segtype[segnum]
    
    def _indexSegnums(self):
#         logging.info('indexing segment type numbers...')
        # index tags
        for tag in self.tagset.getAllTags():
            tagnum = self.tagset.getTagnum4Tag(tag)
            for p in self.patternsList:
                segnum = p.tryToMatch(None, tag)
                if segnum >= 0 and tagnum not in self._tagnum2Segnum:
                    self._tagnum2Segnum[tagnum] = segnum
        
        # index lexemes
        for p in self.patternsList:
            if p.lemma:
                for tag in self.tagset.getAllTags():
                    tagnum = self.tagset.getTagnum4Tag(tag)
                    if not (p.lemma, tagnum) in self._lemmaTagnum2Segnum:
                        segnum = p.tryToMatch(p.lemma, tag)
                        if segnum != -1:
                            self._lemmaTagnum2Segnum[(p.lemma, tagnum)] = segnum
#         logging.info('indexing segment type numbers - done')
#         self._debugSegnums()
    
    def hasSegtype(self, segTypeString):
        return segTypeString in self.segtype2Segnum
    
    def getSegnum4Segtype(self, segTypeString):
        return self.segtype2Segnum[segTypeString]
    
    def lexeme2Segnum(self, lemma, tagnum):
        res = self._lemmaTagnum2Segnum.get((lemma, tagnum), None)
        if not res:
            res = self._tagnum2Segnum.get(tagnum, None)
        return res
#         for p in self.patternsList:
#             res = p.tryToMatch(lemma, tag)
#             if res >= 0:
#                 return res
#         return None
    
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
