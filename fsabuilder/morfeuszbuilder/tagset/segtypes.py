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
        
        self.segtypes = set()
        self.segtype2Segnum = {}
        self.segnum2Segtype = {}
        self.patternsList = []
        
        self._tagnum2Segnum = {}
        self._lemmaTagnum2Segnum = {}
        
        self._readSegtypes(segrulesConfigFile)
        self._readLexemes(segrulesConfigFile)
        self._readTags(segrulesConfigFile)
        self._indexSegnums()
        
        print self._lemmaTagnum2Segnum
        print self._tagnum2Segnum
        
        print self.segnum2Segtype
        
    def _validate(self, msg, lineNum, cond):
        if not cond:
            raise exceptions.ConfigFileException(self.filename, lineNum, msg)
    
    def _readSegtypes(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('segment types'):
            assert type(line) == unicode
            self._validate(
                           u'Segment type must be a single word', 
                           lineNum,
                           re.match(r'^\w+$', line))
            self._validate(
                           u'Segment type already defined: "%s"' % line, 
                           lineNum,
                           line not in self.segtypes)
            self.segtypes.add(line)
                
    
    def _readTags(self, segrulesConfigFile):
        gotWildcardPattern = False
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('tags'):
            splitLine = re.split(r'\s+', line.strip())
            self._validate(
                           u'Line in [tags] section must contain exactly two fields - segment type and tag pattern', 
                           lineNum,
                           len(splitLine) == 2)
            segtype, pattern = splitLine
            self._validate(
                           u'Undeclared segment type: "%s"' % segtype,
                           lineNum,
                           segtype in self.segtypes)
            self._validate(
                           u'Segment type must be a lowercase alphanumeric with optional underscores',
                           lineNum,
                           re.match(r'[a-z_]+', segtype))
            self._validate(
                           u'Pattern must contain only ":", "%", "." and lowercase alphanumeric letters',
                           lineNum,
                           re.match(r'[a-z_\.\:\%]+', pattern))
            
            self._validate(
                           u'Pattern that matches everything must be the last one',
                           lineNum - 1,
                           not gotWildcardPattern)
            
            if segtype in self.segtype2Segnum:
                segnum = self.segtype2Segnum[segtype]
            else:
                segnum = len(self.segtype2Segnum)
                self.segtype2Segnum[segtype] = segnum
            
            segtypePattern = SegtypePattern(None, pattern, segnum)
            
            self._validate(
                           u'There is no tag that matches pattern "%s".' % pattern,
                           lineNum,
                           any([segtypePattern.tryToMatch(None, tag) != -1 for tag in self.tagset.getAllTags()]))
            
            self.patternsList.append(segtypePattern)
            
            gotWildcardPattern = gotWildcardPattern or pattern == '%'
        
        self.segnum2Segtype = dict([(v, k) for (k, v) in self.segtype2Segnum.iteritems()])
    
    def _readLexemes(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('lexemes'):
            segtype, pattern = line.strip().split('\t')
            self._validate(
                           u'Undeclared segment type: "%s"' % segtype,
                           lineNum,
                           segtype in self.segtypes)
            self._validate(
                           u'Segment type must be a lowercase alphanumeric with optional underscores',
                           lineNum,
                           re.match(r'[a-z_]+', segtype))
            self._validate(
                           u'Pattern must contain encodedForm and part-of-speech fields',
                           lineNum,
                           re.match(r'.+?\:[a-z_]+', pattern, re.U))
            
            if segtype in self.segtype2Segnum:
                segnum = self.segtype2Segnum[segtype]
            else:
                segnum = len(self.segtype2Segnum)
                self.segtype2Segnum[segtype] = segnum
            
            lemma, pos = pattern.split(':', 1)
            
            segtypePattern = SegtypePattern(lemma, pos + ':%', segnum)
            
            self._validate(
                           u'There is no tag that matches pattern "%s".' % (pos + ':%'),
                           lineNum,
                           any([segtypePattern.tryToMatch(lemma, tag) != -1 for tag in self.tagset.getAllTags()]))
            
            self.patternsList.append(segtypePattern)
    
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
    
class SegtypePattern(object):
    
    def __init__(self, lemma, pattern, segnum):
        self.lemma = lemma
        self.pattern = pattern
        self.segnum = segnum
    
    def tryToMatch(self, lemma, tag):
#         tag2Match = tag + ':' if not tag.endswith(':') else tag
#         print tag2Match
        patterns2Match = []
        patterns2Match.append(self.pattern.replace('%', '.*'))
        patterns2Match.append(re.sub(r'\:\%$', '', self.pattern).replace('%', '.*'))
        if (self.lemma is None or self.lemma == lemma) \
        and any([re.match(p, tag) for p in patterns2Match]):
            return self.segnum
        else:
#             print 'NOT match', lemma.encode('utf8') if lemma else '%', tag, self.segnum
            return -1
