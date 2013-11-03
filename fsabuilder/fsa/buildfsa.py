# -*- coding:utf-8 -*-
'''
Created on 21 paź 2013

@author: mlenart
'''

import sys
import logging
import codecs
import encode
import convertinput
from fsa import FSA
from serializer import VLengthSerializer2, VLengthSerializer3
from visualizer import Visualizer
from optparse import OptionParser

class OutputFormat():
    BINARY = 'BINARY'
    CPP = 'CPP'

class InputFormat():
    ENCODED = 'ENCODED'
    POLIMORF = 'POLIMORF'
    PLAIN = 'PLAIN'

class FSAType():
    MORPH = 'MORPH'
    SPELL = 'SPELL'

def parseOptions():
    """
    Parses commandline args
    """
    parser = OptionParser()
    parser.add_option('-i', '--input-file',
                        dest='inputFile',
                        metavar='FILE',
                        help='path to input file')
    parser.add_option('-o', '--output-file',
                        dest='outputFile',
                        metavar='FILE',
                        help='path to output file')
    parser.add_option('-t', '--fsa-type',
                        dest='fsaType',
                        help='result FSA type - MORPH (for morphological analysis) or SPELL (for simple spell checker)')
    parser.add_option('--input-format',
                        dest='inputFormat',
                        help='input format - ENCODED, POLIMORF or PLAIN')
    parser.add_option('--output-format',
                        dest='outputFormat',
                        help='output format - BINARY or CPP')
    parser.add_option('--use-arrays',
                        dest='useArrays',
                        action='store_true',
                        default=False,
                        help='store states reachable by 2 transitions in arrays (should speed up recognition)')
    parser.add_option('--visualize',
                        dest='visualize',
                        action='store_true', 
                        default=False,
                        help='visualize result')
    parser.add_option('--train-file',
                        dest='trainFile',
                        help='A text file used for training. Should contain words from some large corpus - one word in each line')
    parser.add_option('--debug',
                        dest='debug',
                        action='store_true',
                        default=False,
                        help='output some debugging info')
    
    opts, args = parser.parse_args()
    
    if None in [opts.inputFile, opts.outputFile, opts.outputFormat, opts.inputFormat, opts.fsaType]:
        parser.print_help()
        exit(1)
    if not opts.outputFormat.upper() in [OutputFormat.BINARY, OutputFormat.CPP]:
        logging.error('output format must be one of ('+str([OutputFormat.BINARY, OutputFormat.CPP])+')')
        parser.print_help()
        exit(1)
    if not opts.inputFormat.upper() in [InputFormat.ENCODED, InputFormat.POLIMORF, InputFormat.PLAIN]:
        logging.error('input format must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF, InputFormat.PLAIN])+')')
        parser.print_help()
        exit(1)
    if not opts.fsaType.upper() in [FSAType.MORPH, FSAType.SPELL]:
        logging.error('input format must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF])+')')
        parser.print_help()
        exit(1)
    if opts.inputFormat.upper() == FSAType.MORPH \
        and not opts.inputFormat.upper() in [InputFormat.ENCODED, InputFormat.POLIMORF]:
        logging.error('input format for morph analysis FSA must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF])+')')
        parser.print_help()
        exit(1)
    if opts.inputFormat.upper() == FSAType.SPELL \
        and not opts.inputFormat.upper() in [InputFormat.PLAIN]:
        logging.error('input format for simple spelling FSA must be '+InputFormat.PLAIN)
        parser.print_help()
        exit(1)
    return opts

def readEncodedInput(inputFile):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for line in f:
            word, interps = line.strip().split()
            yield word, interps.split(u'|')

def readPolimorfInput(inputFile, encoder):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for entry in convertinput.convertPolimorf(f, lambda (word, interp): encoder.word2SortKey(word)):
            yield entry

def readPlainInput(inputFile, encoder):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for line in sorted(f, key=encoder.word2SortKey):
            word = line.strip()
            yield word, ''

def readTrainData(trainFile):
    with codecs.open(trainFile, 'r', 'utf8') as f:
        for line in f:
            yield line.strip()

if __name__ == '__main__':
    opts = parseOptions()
    if opts.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    encoder = encode.Encoder()
    fsa = FSA(encoder)
    
    inputData = {
                 InputFormat.ENCODED: readEncodedInput(opts.inputFile),
                 InputFormat.POLIMORF: readPolimorfInput(opts.inputFile, encoder),
                 InputFormat.PLAIN: readPlainInput(opts.inputFile, encoder)
                 }[opts.inputFormat]
    
    logging.info('feeding FSA with data ...')
    fsa.feed(inputData)
    if opts.trainFile:
        logging.info('training with '+opts.trainFile+' ...')
        fsa.train(readTrainData(opts.trainFile))
        logging.info('done training')
    serializer = VLengthSerializer3(fsa, useArrays=opts.useArrays)
    logging.info('states num: '+str(fsa.getStatesNum()))
    logging.info('transitions num: '+str(fsa.getTransitionsNum()))
    logging.info('accepting states num: '+str(len([s for s in fsa.initialState.dfs(set()) if s.isAccepting()])))
    logging.info('sink states num: '+str(len([s for s in fsa.initialState.dfs(set()) if len(s.transitionsMap.items()) == 0])))
    logging.info('array states num: '+str(len([s for s in fsa.dfs() if s.serializeAsArray])))
    {
     OutputFormat.CPP: serializer.serialize2CppFile,
     OutputFormat.BINARY: serializer.serialize2BinaryFile
     }[opts.outputFormat](opts.outputFile)
    logging.info('size: '+str(fsa.initialState.reverseOffset))
#     for s in fsa.dfs():
#         logging.debug('%d %s' % (s.offset, str(s.transitionsMap)))
#     for s in fsa.initialState.dfs(set()):
#         logging.info(s.offset)
    if opts.visualize:
        Visualizer().visualize(fsa)

