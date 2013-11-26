'''
Created on Nov 7, 2013

@author: mlenart
'''

import codecs
import logging

class Lemma(object):
    
    def __init__(self, cutLength, suffixToAdd, casePattern):
        self.cutLength = cutLength
        self.suffixToAdd = suffixToAdd
        self.casePattern = casePattern

class Interpretation(object):
    
    def __init__(self, orth, base, tagnum, namenum, typenum):
        assert type(orth) == unicode
        assert type(base) == unicode
        root = u''
        for o, b in zip(orth, base):
            if o.lower() == b.lower():
                root += b
            else:
                break
        cutLength = len(orth) - len(root)
        self.lemma = Lemma(
                           cutLength=cutLength,
                           suffixToAdd=base[len(root):],
                           casePattern = [c == c.upper() for c in root])
        self.tagnum = tagnum
        self.namenum = namenum
        self.typenum = typenum
    
    def getSortKey(self):
        return (
                self.lemma.cutLength, 
                tuple(self.lemma.suffixToAdd), 
                tuple(self.lemma.casePattern), 
                self.tagnum, 
                self.namenum)
    
    def __eq__(self, other):
        if isinstance(other, Interpretation):
            return self.getSortKey() == other.getSortKey()
        else:
            return False
    
    def __hash__(self):
        return hash(self.getSortKey())

class Tagset(object):
    
    TAGS = 1
    NAMES = 2
    SEP = '\t'
    
    def __init__(self, filename, encoding='utf8'):
        self.tag2tagnum = {}
        self.name2namenum = {}
        self._doInit(filename, encoding)
        print self.tag2tagnum
        print self.name2namenum
    
    def _doInit(self, filename, encoding):
        addingTo = None
        with codecs.open(filename, 'r', encoding) as f:
            for line in f:
                line = line.strip('\n')
                if line == u'[TAGS]':
                    addingTo = Tagset.TAGS
                elif line == u'[NAMES]':
                    addingTo = Tagset.NAMES
                elif line and not line.startswith(u'#'):
                    assert addingTo in [Tagset.TAGS, Tagset.NAMES]
                    res = {Tagset.TAGS: self.tag2tagnum,
                           Tagset.NAMES: self.name2namenum}[addingTo]
                    tagNum = line.split(Tagset.SEP)[0]
                    tag = line.split(Tagset.SEP)[1]
                    assert tag not in res
                    res[tag] = int(tagNum)
