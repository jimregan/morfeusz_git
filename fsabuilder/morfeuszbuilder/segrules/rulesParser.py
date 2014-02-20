
from pyparsing import *
ParserElement.enablePackrat()
from morfeuszbuilder.tagset import segtypes
from morfeuszbuilder.utils import configFile, exceptions
from morfeuszbuilder.segrules import preprocessor, rules, rulesManager
import codecs
import re

import itertools
import logging
from morfeuszbuilder.segrules import rulesNFA

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
                raise exceptions.ConfigFileException(segtypesConfigFile.filename, lineNum, u'Error in [options] section: %s' % str(ex))
        return res
    
    def parse(self, filename):
        res = rulesManager.RulesManager()
        
        segtypesConfigFile = configFile.ConfigFile(filename, ['options', 'combinations', 'tags', 'lexemes', 'segment types'])
        key2Defs = self._getKey2Defs(segtypesConfigFile)
        segtypesHelper = segtypes.Segtypes(self.tagset, segtypesConfigFile)
        
        def2Key = {}
        for key, defs in key2Defs.iteritems():
            for define in defs:
                def2Key[define] = key
        
        firstNFA = None
        for defs in itertools.product(*key2Defs.values()):
            key2Def = dict([(def2Key[define], define) for define in defs])
            nfa = rulesNFA.RulesNFA()
            if not firstNFA:
                firstNFA = nfa
            combinationEnumeratedLines = segtypesConfigFile.enumerateLinesInSection('combinations')
            combinationEnumeratedLines = list(preprocessor.preprocess(combinationEnumeratedLines, defs))
            for rule in self._doParse(combinationEnumeratedLines, segtypesHelper):
                rule.addToNFA(nfa)
            dfa = nfa.convertToDFA()
            res.addDFA4Options(key2Def, dfa)
        return res
    
    def _doParse(self, combinationEnumeratedLines, segtypesHelper):
        for lineNum, line in combinationEnumeratedLines:
            if not line.startswith('#'):
                yield self._doParseOneLine(lineNum, line, segtypesHelper)
    
    def _createNewTagRule(self, segtype, lineNum, line, segtypesHelper):
        if not segtypesHelper.hasSegtype(segtype):
            raise exceptions.ConfigFileException(segtypesHelper.filename, lineNum, u'%s - invalid segment type: %s' % (line, segtype))
        else:
#             return rules.TagRule(segtype)
            return rules.TagRule(segtypesHelper.getSegnum4Segtype(segtype))
    
    def _doParseOneLine(self, lineNum, line, segtypesHelper):
        rule = Forward()
        tagRule = Word(alphanums+'_>')
        parenRule = Suppress('(') + rule + Suppress(')')
        atomicRule = tagRule ^ parenRule
        zeroOrMoreRule = atomicRule + Suppress('*')
        oneOrMoreRule = atomicRule + Suppress('+')
        unaryRule = atomicRule ^ zeroOrMoreRule ^ oneOrMoreRule
        oneOfRule = delimitedList(unaryRule, delim='|')
        complexRule = unaryRule ^ oneOfRule
        concatRule = OneOrMore(complexRule)
        rule << concatRule
        
        tagRule.setParseAction(lambda string, loc, toks: self._createNewTagRule(toks[0], lineNum, line, segtypesHelper))
#         parenRule.setParseAction(lambda string, loc, toks: toks[0])
        zeroOrMoreRule.setParseAction(lambda string, loc, toks: rules.ZeroOrMoreRule(toks[0]))
        oneOrMoreRule.setParseAction(lambda string, loc, toks: rules.ConcatRule([toks[0], rules.ZeroOrMoreRule(toks[0])]))
        oneOfRule.setParseAction(lambda string, loc, toks: rules.OrRule(toks))
        concatRule.setParseAction(lambda string, loc, toks: toks[0] if len(toks) == 1 else rules.ConcatRule(toks))
        parsedRule = rule.parseString(line, parseAll=True)[0]
        return parsedRule
