'''
Created on 17 lut 2014

@author: mlenart
'''

import codecs
from morfeuszbuilder.utils.exceptions import FSABuilderException

class Tagset(object):
    
    TAGS = 1
    NAMES = 2
    SEP = '\t'
    
    def __init__(self, filename=None, encoding='utf8'):
        self._tag2tagnum = {}
        self._name2namenum = {}
        if filename:
            self._doInit(filename, encoding)
        self._tagnum2tag = dict(map(lambda (k, v): (v, k), self._tag2tagnum.iteritems()))
    
    def _doInit(self, filename, encoding):
        addingTo = None
        with codecs.open(filename, 'r', encoding) as f:
            for linenum, line in enumerate(f, start=1):
                line = line.strip('\n\r')
                if line == u'[TAGS]':
                    addingTo = Tagset.TAGS
                elif line == u'[NAMES]':
                    addingTo = Tagset.NAMES
                elif line and not line.startswith(u'#'):
                    if not addingTo in [Tagset.TAGS, Tagset.NAMES]:
                        raise FSABuilderException('"%s" - text outside [TAGS] section in tagset file line %d' % (line, linenum))
                    res = {Tagset.TAGS: self._tag2tagnum,
                           Tagset.NAMES: self._name2namenum}[addingTo]
                    tagNum = line.split(Tagset.SEP)[0]
                    tag = line.split(Tagset.SEP)[1]
                    if tag in res:
                        raise FSABuilderException('duplicate tag: "%s"' % tag)
                    res[tag] = int(tagNum)
    
    def getAllTags(self):
        return self._tag2tagnum.keys()
    
    def getTagnum4Tag(self, tag):
        if tag in self._tag2tagnum:
            return self._tag2tagnum[tag]
        else:
            raise FSABuilderException('invalid tag: "%s"' % tag)
    
    def getNamenum4Name(self, name):
        if name in self._name2namenum:
            return self._name2namenum[name]
        else:
            raise FSABuilderException('invalid name type: "%s"' % name)
    
    def getTag4Tagnum(self, tagnum):
        return self._tagnum2tag[tagnum]
