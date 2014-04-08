# -*- coding:utf-8 -*-
'''
Created on 23 sty 2014

@author: mlenart
'''
import re
from pyparsing import *
from morfeuszbuilder.utils import exceptions
from pyparseString import pyparseString

identifier = Word(alphas, bodyChars=alphanums+u'_>*+{},')
define = Keyword('#define').suppress() + identifier + Optional(Suppress('(') + identifier + Suppress(')')) + restOfLine + LineEnd() + StringEnd()
ifdef = Keyword('#ifdef').suppress() + identifier + LineEnd() + StringEnd()
endif = Keyword('#endif').suppress() + LineEnd() + StringEnd()

class NonArgDefine(object):
    
    def __init__(self, name, val):
        self.name = name
        self.val = val
    
    def hasArg(self):
        return False

class ArgDefine(object):
    
    def __init__(self, name, arg, val):
        self.name = name
        self.arg = arg
        self.val = val
    
    def hasArg(self):
        return True
    
    def __str__(self):
        return '%s(%s) %s' % (self.name, self.arg, self.val)

def _tryToSubstituteArgDefine(s, t, defines):
    defineName = t[0]
    substituteValue = t[1]
    if defineName in defines and defines[defineName].hasArg():
        define = defines[defineName]
        return re.sub(r'\b%s\b' % define.arg, substituteValue, define.val)
    elif defineName in defines:
        return '%s ( %s )' % (defines[defineName].val, substituteValue)
    else:
        return ' '.join(t)

def _tryToSubstituteNonArgDefine(s, t, defines):
    defineName = t[0]
    
    if defineName in defines and not defines[defineName].hasArg():
        return defines[defineName].val
    else:
        return defineName

def _processLine(lineNum, line, defines, filename):
    if line.strip():
        
        rule = Forward()
        defineInstance = Forward()
        localId = identifier.copy()
        weakLiteral = CaselessLiteral('!weak')
        
        rule << OneOrMore(defineInstance ^ localId ^ Word('*|+?>') ^ (Literal('(') + rule + Literal(')')) ^ weakLiteral)
        defineInstance << localId + Suppress('(') + rule + Suppress(')')
        
        rule.setParseAction(lambda s, l, t: ' '.join(t))
        defineInstance.setParseAction(lambda s, l, t: _tryToSubstituteArgDefine(s, t, defines))
        localId.setParseAction(lambda s, l, t: _tryToSubstituteNonArgDefine(s, t, defines))
        return pyparseString(rule, lineNum, line, filename)[0]
    else:
        return line

def preprocess(inputLines, defs, filename):
    defines = {}
    ifdefsStack = []
    wasElse = False
    for lineNum, line in inputLines:
        if line.startswith('#define'):
            parsedDefine = list(pyparseString(define, lineNum, line, filename))
            if len(parsedDefine) == 2:
                name, val = parsedDefine
                defines[name] = NonArgDefine(name, val)
            else:
                name, arg, val = parsedDefine
                localDefines = defines.copy()
                localDefines[arg] = NonArgDefine(arg, arg)
                val = _processLine(lineNum, val, localDefines, filename)
                defines[name] = ArgDefine(name, arg, val)
        elif line.startswith('#ifdef'):
            name = pyparseString(ifdef, lineNum, line, filename)[0]
#             name = ifdef.parseString(line)[0]
            ifdefsStack.append(name)
        elif line.startswith('#else'):
            ifdefsStack.pop()
            wasElse = True
        elif line.startswith('#endif'):
            if not wasElse:
                ifdefsStack.pop()
            wasElse = False
        elif line.startswith('#'):
            yield lineNum, line
        elif len(ifdefsStack) == 0 or all(map(lambda name: name in defs, ifdefsStack)):
            yield lineNum, _processLine(lineNum, line, defines, filename)
        