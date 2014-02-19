'''
Created on 17 lut 2014

@author: mlenart
'''

import codecs

class Tagset(object):
    
    TAGS = 1
    NAMES = 2
    SEP = '\t'
    
    def __init__(self, filename=None, encoding='utf8'):
        self.tag2tagnum = {}
        self.name2namenum = {}
        if filename:
            self._doInit(filename, encoding)
        self.tagnum2tag = dict(map(lambda (k, v): (v, k), self.tag2tagnum.iteritems()))
    
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
    
    def getTag4Tagnum(self, tagnum):
        return self.tagnum2tag[tagnum]
