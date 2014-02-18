
from pyparsing import *
from morfeuszbuilder.tagset import segtypes
from morfeuszbuilder.utils import configFile
from morfeuszbuilder.segrules import preprocessor
import codecs
import re

import itertools
import logging
import segsfsa

# header = Suppress('[') + Word(alphas, bodyChars=alphanums+'_') + Suppress(']')
# define = Keyword('#define').suppress() + identifier + Optional(Suppress('(') + identifier + Suppress(')')) + restOfLine + LineEnd() + StringEnd()
# ifdef = Keyword('#ifdef').suppress() + identifier + LineEnd() + StringEnd()
# endif = Keyword('#endif').suppress() + LineEnd() + StringEnd()

def doprint(toks):
    print toks

class RulesParser(object):
    
    def __init__(self, tagset):
        self.tagset = tagset
    
    def _getKey2Defs(self, segtypesConfigFile):
        res = {}
        for lineNum, line in segtypesConfigFile.enumerateLinesInSection('options'):
            lineToParse = Word(alphanums+'_') + Suppress('=') + Group(OneOrMore(Word(alphanums+'_'))) + LineEnd().suppress()
            try:
                key, defs = lineToParse.parseString(line)
                res[key] = tuple(defs)
            except Exception as ex:
                raise configFile.ConfigFileException(segtypesConfigFile.filename, lineNum, u'Error in [options] section: %s' % str(ex))
        return res
    
    def parse(self, filename):
        res = []
        
        segtypesConfigFile = configFile.ConfigFile(filename, ['options', 'combinations', 'tags', 'lexemes'])
        key2Defs = self._getKey2Defs(segtypesConfigFile)
        segtypesHelper = segtypes.Segtypes(self.tagset, segtypesConfigFile)
        
        def2Key = {}
        for key, defs in key2Defs.iteritems():
            for define in defs:
                def2Key[define] = key
        
        for defs in itertools.product(*key2Defs.values()):
            key2Def = dict([(def2Key[define], define) for define in defs])
            fsa = segsfsa.SegmentsFSA(key2Def)
            combinationEnumeratedLines = segtypesConfigFile.enumerateLinesInSection('combinations')
            combinationEnumeratedLines = list(preprocessor.preprocess(combinationEnumeratedLines, defs))
            for rule in self._doParse(combinationEnumeratedLines, segtypesHelper):
                fsa.addSegmentRule(rule)
            res.append(fsa)
        return res
    
    def _doParse(self, combinationEnumeratedLines, segtypesHelper):
        for lineNum, line in combinationEnumeratedLines:
            if not line.startswith('#'):
                yield self._doParseOneLine(lineNum, line, segtypesHelper)
    
    def _doParseOneLine(self, lineNum, line, segtypesHelper):
        rule = Forward()
        tagRule = Word(alphanums+'_')
        ignoreOrthRule = tagRule + Suppress('>')
        parenRule = Suppress('(') + rule + Suppress(')')
        atomicRule = tagRule ^ ignoreOrthRule ^ parenRule
        zeroOrMoreRule = atomicRule + Suppress('*')
        oneOrMoreRule = atomicRule + Suppress('+')
        unaryRule = atomicRule ^ zeroOrMoreRule ^ oneOrMoreRule
        oneOfRule = delimitedList(unaryRule, delim='|')
        complexRule = unaryRule ^ oneOfRule
        concatRule = OneOrMore(complexRule)
        rule << concatRule
#         rule << tagRule ^ ignoreOrthRule ^ zeroOrMoreRule ^ oneOrMoreRule ^ orRule ^ concatRule ^ parenRule
        
#         tagRule.setParseAction(lambda s,l,toks: doprint(toks))
#         print lineNum, line
        parsedLine = rule.parseString(line, parseAll=True)
#         print parsedLine
