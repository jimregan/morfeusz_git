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
            for line in f:
                line = line.strip('\n')
                if line == u'[TAGS]':
                    addingTo = Tagset.TAGS
                elif line == u'[NAMES]':
                    addingTo = Tagset.NAMES
                elif line and not line.startswith(u'#'):
                    assert addingTo in [Tagset.TAGS, Tagset.NAMES]
                    res = {Tagset.TAGS: self._tag2tagnum,
                           Tagset.NAMES: self._name2namenum}[addingTo]
                    tagNum = line.split(Tagset.SEP)[0]
                    tag = line.split(Tagset.SEP)[1]
                    assert tag not in res
                    res[tag] = int(tagNum)
    
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
