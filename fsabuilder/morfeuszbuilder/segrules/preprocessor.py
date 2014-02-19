'''
Created on 23 sty 2014

@author: mlenart
'''
import re
from pyparsing import *

identifier = Word(alphas, bodyChars=alphanums+'_')
token = Word(alphas, bodyChars=alphanums+'_+>')
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

def _processLine(line, defines):
    if line.strip():
        
        rule = Forward()
        defineInstance = Forward()
        localId = identifier.copy()
        
        rule << OneOrMore(defineInstance ^ localId ^ Word('*|+?>') ^ (Literal('(') + rule + Literal(')')))
        defineInstance << localId + Suppress('(') + rule + Suppress(')')
        
        rule.setParseAction(lambda s, l, t: ' '.join(t))
        defineInstance.setParseAction(lambda s, l, t: _tryToSubstituteArgDefine(s, t, defines))
        localId.setParseAction(lambda s, l, t: _tryToSubstituteNonArgDefine(s, t, defines))
        return rule.parseString(line, parseAll=True)[0]
    else:
        return line

def preprocess(inputLines, defs):
    defines = {}
    ifdefsStack = []
    for lineNum, line in inputLines:
        if line.startswith('#define'):
            parsedDefine = list(define.parseString(line))
            if len(parsedDefine) == 2:
                name, val = parsedDefine
                defines[name] = NonArgDefine(name, val)
            else:
                name, arg, val = parsedDefine
                localDefines = defines.copy()
                localDefines[arg] = NonArgDefine(arg, arg)
                val = _processLine(val, localDefines)
                defines[name] = ArgDefine(name, arg, val)
        elif line.startswith('#ifdef'):
            name = ifdef.parseString(line)[0]
            ifdefsStack.append(name)
        elif line.startswith('#endif'):
            ifdefsStack.pop()
        elif line.startswith('#'):
            yield lineNum, line
        elif len(ifdefsStack) == 0 or all(map(lambda name: name in defs, ifdefsStack)):
            yield lineNum, _processLine(line, defines)
        