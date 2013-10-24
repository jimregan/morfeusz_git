'''
Created on Oct 23, 2013

@author: mlenart
'''
import sys
import fileinput
import logging
from encode import Encoder

def _encodeInterp(orth, base, tag, name):
    removePrefix = 0
    root = u''
    for o, b in zip(orth, base):
        if o == b:
            root += o
        else:
            break
    removeSuffixNum = len(orth) - len(root)
    addSuffix = base[len(root):]
    return u'+'.join([
        chr(ord('A')+removePrefix) + chr(ord('A')+removeSuffixNum) + addSuffix, 
        tag,
        name])

def _parsePolimorf(inputLines):
    for line0 in inputLines:
        line = line0.strip(u'\n')
        if line:
#             print line
            orth, base, tag, name = line.split(u'\t')
            yield (orth, _encodeInterp(orth, base, tag, name))

def _sortAndMergeParsedInput(inputData, key=lambda k: k):
    logging.info('sorting input...')
    entries = list(inputData)
    entries.sort(key=key)
    logging.info('done sorting')
    prevOrth = None
    prevInterps = None
    for orth, interp in entries:
        if prevOrth and prevOrth == orth:
            prevInterps.append(interp)
        else:
            if prevOrth:
                yield (prevOrth, sorted(set(prevInterps)))
            prevOrth = orth
            prevInterps = [interp]

def convertPolimorf(inputLines, sortKey=lambda k: k):
    for orth, interps in _sortAndMergeParsedInput(_parsePolimorf(inputLines), key=sortKey):
        yield orth, interps

def _decodeInputLines(rawInputLines, encoding):
    for line in rawInputLines:
        yield line.decode(encoding)

if __name__ == '__main__':
    encoder = Encoder()
    for orth, interps in convertPolimorf(_decodeInputLines(fileinput.input(), 'utf8'), lambda (orth, interp): encoder.word2SortKey(orth)):
        print u'\t'.join([orth, u'|'.join(interps)]).encode('utf8')
