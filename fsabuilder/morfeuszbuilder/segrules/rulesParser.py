
from pyparsing import *
ParserElement.enablePackrat()
from morfeuszbuilder.tagset import segtypes
from morfeuszbuilder.utils import configFile, exceptions
from morfeuszbuilder.segrules import preprocessor, rules, rulesManager, pyparseString
import codecs
import re

import itertools
import logging
from morfeuszbuilder.segrules import rulesNFA

class RulesParser(object):
    
    PARSE4GENERATOR = 1
    PARSE4ANALYZER = 2
    
    def __init__(self, tagset, rulesType):
        self.tagset = tagset
        assert rulesType in (RulesParser.PARSE4GENERATOR, RulesParser.PARSE4ANALYZER)
        self.rulesType = rulesType
    
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
        
        segtypesConfigFile = configFile.ConfigFile(filename, ['options', 'combinations', 'generator combinations', 'tags', 'lexemes', 'segment types'])
        key2Defs = self._getKey2Defs(segtypesConfigFile)
        segtypesHelper = segtypes.Segtypes(self.tagset, segtypesConfigFile)
        
        res = rulesManager.RulesManager(segtypesHelper)
        
        def2Key = {}
        for key, defs in key2Defs.iteritems():
            for define in defs:
                def2Key[define] = key
        
        firstNFA = None
        for idx, defs in enumerate(itertools.product(*key2Defs.values())):
            key2Def = dict([(def2Key[define], define) for define in defs])
#             print key2Def
            nfa = rulesNFA.RulesNFA()
            if not firstNFA:
                firstNFA = nfa
            section = 'combinations' if self.rulesType == RulesParser.PARSE4ANALYZER else 'generator combinations'
            combinationEnumeratedLines = segtypesConfigFile.enumerateLinesInSection(section, ignoreComments=False)
            combinationEnumeratedLines = list(preprocessor.preprocess(combinationEnumeratedLines, defs, filename))
            for rule in self._doParse(combinationEnumeratedLines, segtypesHelper, filename):
                if rule.allowsEmptySequence():
                    raise exceptions.ConfigFileException(
                                                     filename, 
                                                     rule.linenum, 
                                                     'This rule allows empty segments sequence to be accepted')
                rule.addToNFA(nfa)
#                 nfa.debug()
            try:
                dfa = nfa.convertToDFA()
                res.addDFA(key2Def, dfa)
            except rulesNFA.InconsistentStateWeaknessException as ex:
                raise exceptions.ConfigFileException(
                                                     filename, 
                                                     ex.weakState.rule.linenum, 
                                                     'conflicts with rule at line %d. Segmentation for some chunks can be both weak and non-weak which is illegal.' % ex.nonWeakState.rule.linenum)
#             print '********* DFA **************'
#             dfa.debug()
#             print dfa.tryToRecognize(bytearray([14]))
            if idx == 0:
                res.setDefaultOptions(key2Def)
        return res
    
    def _doParse(self, combinationEnumeratedLines, segtypesHelper, filename):
        for lineNum, line in combinationEnumeratedLines:
            if not line.startswith('#'):
                yield self._doParseOneLine(lineNum, line, segtypesHelper, filename)
    
    def _createNewTagRule(self, segtype, shiftOrth, lineNum, line, segtypesHelper):
        if not segtypesHelper.hasSegtype(segtype):
            raise exceptions.ConfigFileException(segtypesHelper.filename, lineNum, u'%s - invalid segment type: %s' % (line, segtype))
        else:
#             return rules.TagRule(segtype)
            return rules.TagRule(segtypesHelper.getSegnum4Segtype(segtype), shiftOrth, segtype, lineNum)
    
    def _createQuantRule1(self, child, quantity, lineNum, line, segtypesHelper):
        if quantity <= 0:
            raise exceptions.ConfigFileException(segtypesHelper.filename, lineNum, u'%s - invalid quantity: %d' % (line, quantity))
        else:
            return rules.ConcatRule(quantity * [child], lineNum)
    
    def _createQuantRule2(self, child, leftN, rightN, lineNum, line, segtypesHelper):
        if leftN > rightN or (leftN, rightN) == (0, 0):
            raise exceptions.ConfigFileException(segtypesHelper.filename, lineNum, u'%s - invalid quantities: %d %d' % (line, leftN, rightN))
        elif leftN == 0:
            children = [rules.OptionalRule(child, lineNum)]
            for n in range(2, rightN + 1):
                children.append(self._createQuantRule1(child, n, lineNum, line, segtypesHelper))
            return rules.OrRule(children, lineNum)
        else:
            children = [self._createQuantRule1(child, n, lineNum, line, segtypesHelper) for n in range(leftN, rightN + 1)]
            return rules.OrRule(children, lineNum)
    
    def _createQuantRule3(self, child, quantity, lineNum, line, segtypesHelper):
        if quantity <= 0:
            raise exceptions.ConfigFileException(segtypesHelper.filename, lineNum, u'%s - invalid quantity: %d' % (line, quantity))
        else:
            return rules.ConcatRule(
                                    [
                                        rules.ConcatRule(quantity * [child], lineNum),
                                        rules.ZeroOrMoreRule(child, lineNum)
                                    ],
                                    lineNum)
    
    def _doParseOneLine(self, lineNum, line, segtypesHelper, filename):
        rule = Forward()
        tagRule = Word(alphanums+'_')
        shiftOrthRule = Word(alphanums+'_') + Suppress('>')
        parenRule = Suppress('(') + rule + Suppress(')')
        atomicRule = tagRule ^ shiftOrthRule ^ parenRule
        zeroOrMoreRule = atomicRule + Suppress('*')
        oneOrMoreRule = atomicRule + Suppress('+')
        optionalRule = atomicRule + Suppress('?')
        quantRule1 = atomicRule + Suppress('{') + Word(nums) + Suppress('}')
        quantRule2 = atomicRule + Suppress('{') + Word(nums) + Suppress(',') + Word(nums) + Suppress('}')
        quantRule3 = atomicRule + Suppress('{') + Word(nums) + Suppress(',') + Suppress('}')
        unaryRule = atomicRule ^ zeroOrMoreRule ^ oneOrMoreRule ^ optionalRule ^ quantRule1 ^ quantRule2 ^ quantRule3
        oneOfRule = delimitedList(unaryRule, delim='|')
        complexRule = unaryRule ^ oneOfRule
        if self.rulesType == RulesParser.PARSE4ANALYZER:
            concatRule = OneOrMore(complexRule)
        else:
            concatRule = ZeroOrMore(shiftOrthRule) + tagRule
        rule << concatRule + Optional(CaselessLiteral('!weak'))
        
        tagRule.setParseAction(lambda string, loc, toks: self._createNewTagRule(toks[0], False, lineNum, line, segtypesHelper))
        shiftOrthRule.setParseAction(lambda string, loc, toks: self._createNewTagRule(toks[0], True, lineNum, line, segtypesHelper))
#         parenRule.setParseAction(lambda string, loc, toks: toks[0])
        zeroOrMoreRule.setParseAction(lambda string, loc, toks: rules.ZeroOrMoreRule(toks[0], lineNum))
        quantRule1.setParseAction(lambda string, loc, toks: self._createQuantRule1(toks[0], int(toks[1], 10), lineNum, line, segtypesHelper))
        quantRule2.setParseAction(lambda string, loc, toks: self._createQuantRule2(toks[0], int(toks[1], 10), int(toks[2], 10), lineNum, line, segtypesHelper))
        quantRule3.setParseAction(lambda string, loc, toks: self._createQuantRule3(toks[0], int(toks[1], 10), lineNum, line, segtypesHelper))
        optionalRule.setParseAction(lambda string, loc, toks: rules.OptionalRule(toks[0], lineNum))
        oneOrMoreRule.setParseAction(lambda string, loc, toks: rules.ConcatRule([toks[0], rules.ZeroOrMoreRule(toks[0], lineNum)], lineNum))
        oneOfRule.setParseAction(lambda string, loc, toks: rules.OrRule(toks, lineNum))
        concatRule.setParseAction(lambda string, loc, toks: toks[0] if len(toks) == 1 else rules.ConcatRule(toks, lineNum))
        rule.setParseAction(lambda string, loc, toks: toks[0].setWeak(len(toks) == 2))
        parsedRule = pyparseString.pyparseString(rule, lineNum, line, filename)[0]
        return parsedRule
