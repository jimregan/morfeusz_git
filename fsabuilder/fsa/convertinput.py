'''
Created on Oct 23, 2013

@author: mlenart
'''
import logging
from common import Interpretation

def _sortLines(inputLines, encoder):
    logging.info('sorting input...')
    lines = list(inputLines)
    logging.info('done read data into list')
    lines.sort(key=lambda line: encoder.word2SortKey(line.split('\t')[0]))
    logging.info('done sorting')
    return lines

def _parseLines(inputLines, tagset, encoder):
    for line in inputLines:
        line = line.strip(u'\n')
        if line:
#             print line
            orth, base, tag, name = line.split(u'\t')
            tagnum = tagset.tag2tagnum[tag]
            namenum = tagset.name2namenum[name]
            yield (orth, Interpretation(orth, base, tagnum, namenum, encoder))

def _mergeEntries(inputLines):
    prevOrth = None
    prevInterps = None
    for orth, interp in inputLines:
        orth = orth.lower()
        assert orth
        if prevOrth and prevOrth == orth:
            prevInterps.append(interp)
        else:
            if prevOrth:
                yield (prevOrth, frozenset(prevInterps))
            prevOrth = orth
            prevInterps = [interp]
    yield (prevOrth, frozenset(prevInterps))

def convertPolimorf(inputLines, tagset, encoder):
    for orth, interps in _mergeEntries(_parseLines(_sortLines(inputLines, encoder), tagset, encoder)):
        yield orth, interps
