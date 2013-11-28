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
import common
from fsa import FSA
from serializer import VLengthSerializer1, VLengthSerializer2, SimpleSerializer
from visualizer import Visualizer
from optparse import OptionParser

class OutputFormat():
    BINARY = 'BINARY'
    CPP = 'CPP'

# class InputFormat():
#     ENCODED = 'ENCODED'
#     POLIMORF = 'POLIMORF'
#     PLAIN = 'PLAIN'

class FSAType():
    MORPH = 'MORPH'
    SPELL = 'SPELL'

class SerializationMethod():
    SIMPLE = 'SIMPLE'
    V1 = 'V1'
    V2 = 'V2'

def _parseOptions():
    """
    Parses commandline args
    """
    parser = OptionParser()
    parser.add_option('-i', '--input-file',
                        dest='inputFile',
                        metavar='FILE',
                        help='path to input file')
    parser.add_option('--tagset-file',
                        dest='tagsetFile',
                        metavar='FILE',
                        help='path to the file with tagset')
    parser.add_option('-o', '--output-file',
                        dest='outputFile',
                        metavar='FILE',
                        help='path to output file')
#     parser.add_option('-t', '--fsa-type',
#                         dest='fsaType',
#                         help='result FSA type - MORPH (for morphological analysis) or SPELL (for simple spell checker)')
#     parser.add_option('--input-format',
#                         dest='inputFormat',
#                         help='input format - ENCODED, POLIMORF or PLAIN')
    parser.add_option('--output-format',
                        dest='outputFormat',
                        help='output format - BINARY or CPP')
    parser.add_option('--use-arrays',
                        dest='useArrays',
                        action='store_true',
                        default=False,
                        help='store states reachable by 2 transitions in arrays (should speed up recognition, available only when --serialization-method=V1)')
    parser.add_option('--serialization-method',
                        dest='serializationMethod',
                        help="FSA serialization method: \
                        SIMPLE - fixed-length transitions, fastest and weakest compression \
                        V1 - variable-length transitions, compressed labels - strongest compression \
                        V2 - format similar to the default in Jan Daciuk's fsa package - variable-length transitions, non-compressed labels - good compression, good speed")
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
    parser.add_option('--profile',
                        dest='profile',
                        action='store_true',
                        default=False,
                        help='show profiling graph (required pycallgraph and graphviz')
    
    opts, args = parser.parse_args()
    
    if None in [opts.inputFile, opts.outputFile, opts.outputFormat, opts.tagsetFile, opts.serializationMethod]:
        parser.print_help()
        exit(1)
    if not opts.outputFormat.upper() in [OutputFormat.BINARY, OutputFormat.CPP]:
        logging.error('--output-format must be one of ('+str([OutputFormat.BINARY, OutputFormat.CPP])+')')
        parser.print_help()
        exit(1)
#     if not opts.inputFormat.upper() in [InputFormat.ENCODED, InputFormat.POLIMORF, InputFormat.PLAIN]:
#         logging.error('input format must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF, InputFormat.PLAIN])+')')
#         parser.print_help()
#         exit(1)
#     if not opts.fsaType.upper() in [FSAType.MORPH, FSAType.SPELL]:
#         logging.error('--fsa-type must be one of ('+str([FSAType.MORPH, FSAType.SPELL])+')')
#         parser.print_help()
#         exit(1)
#     if opts.fsaType == FSAType.MORPH and opts.tagsetFile is None:
#         logging.error('must provide tagset file')
#         parser.print_help()
#         exit(1)
    
    if not opts.serializationMethod.upper() in [SerializationMethod.SIMPLE, SerializationMethod.V1, SerializationMethod.V2]:
        logging.error('--serialization-method must be one of ('+str([SerializationMethod.SIMPLE, SerializationMethod.V1, SerializationMethod.V2])+')')
        parser.print_help()
        exit(1)
#     if opts.inputFormat.upper() == FSAType.MORPH \
#         and not opts.inputFormat.upper() in [InputFormat.ENCODED, InputFormat.POLIMORF]:
#         logging.error('input format for morph analysis FSA must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF])+')')
#         parser.print_help()
#         exit(1)
#     if opts.inputFormat.upper() == FSAType.SPELL \
#         and not opts.inputFormat.upper() in [InputFormat.PLAIN]:
#         logging.error('input format for simple spelling FSA must be '+InputFormat.PLAIN)
#         parser.print_help()
#         exit(1)
    return opts

def _readPolimorfInput(inputFile, tagset, encoder):
    with open(inputFile, 'r') as f:
        for entry in convertinput.PolimorfConverter(tagset, encoder, 'utf8').convert(f):
            yield entry

def _readPlainInput(inputFile, encoder):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for line in sorted(f, key=encoder.word2SortKey):
            word = line.strip()
            yield word, ''

def _readTrainData(trainFile):
    with codecs.open(trainFile, 'r', 'utf8') as f:
        for line in f:
            yield line.strip()

def _printStats(fsa):
    acceptingNum = 0
    sinkNum = 0
    arrayNum = 0
    for s in fsa.dfs():
        if s.isAccepting():
            acceptingNum += 1
        if s.transitionsNum == 0:
            sinkNum += 1
        if s.serializeAsArray:
            arrayNum += 1
    logging.info('states num: '+str(fsa.getStatesNum()))
    logging.info('transitions num: '+str(fsa.getTransitionsNum()))
    logging.info('accepting states num: '+str(acceptingNum))
    logging.info('sink states num: '+str(sinkNum))
    logging.info('array states num: '+str(arrayNum))

def buildFromPoliMorf(inputFile, tagsetFile):
    encoder = encode.MorphEncoder()
    tagset = common.Tagset(tagsetFile)
    fsa = FSA(encoder, tagset)
    inputData = _readPolimorfInput(inputFile, tagset, encoder)
    
    fsa.feed(inputData)
    
    _printStats(fsa)
    return fsa

def buildFromPlain(inputFile, tagsetFile):
    pass

def main(opts):
    if opts.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    
    fsa = buildFromPoliMorf(opts.inputFile, opts.tagsetFile)
#     {
#            FSAType.SPELL: buildFromPlain(opts.inputFile),
#            FSAType.MORPH: buildFromPoliMorf(opts.inputFile, opts.tagsetFile)
#            }[opts.fsaType]
    
    if opts.trainFile:
        logging.info('training with '+opts.trainFile+' ...')
        fsa.train(_readTrainData(opts.trainFile))
        logging.info('done training')
    
#     encoder = {
#                FSAType.SPELL: encode.SimpleEncoder(),
#                FSAType.MORPH: encode.MorphEncoder()
#                }[opts.fsaType]
#     
#     fsa = FSA(encoder)
#     
#     inputData = {
#                  FSAType.MORPH: _readPolimorfInput(opts.inputFile, opts.tagsetFile, encoder),
#                  FSAType.SPELL: _readPlainInput(opts.inputFile, encoder)
#                  }[opts.fsaType]
    
#     logging.info('feeding FSA with data ...')
#     fsa.feed(inputData)
#     if opts.trainFile:
#         logging.info('training with '+opts.trainFile+' ...')
#         fsa.train(readTrainData(opts.trainFile))
#         logging.info('done training')
        
    serializer = {
                  SerializationMethod.SIMPLE: SimpleSerializer,
                  SerializationMethod.V1: VLengthSerializer1,
                  SerializationMethod.V2: VLengthSerializer2,
                  }[opts.serializationMethod](fsa)

    {
     OutputFormat.CPP: serializer.serialize2CppFile,
     OutputFormat.BINARY: serializer.serialize2BinaryFile
     }[opts.outputFormat](opts.outputFile)
    logging.info('size: '+str(fsa.initialState.reverseOffset))
    
    if opts.visualize:
        Visualizer().visualize(fsa)

if __name__ == '__main__':
    opts = _parseOptions()
    if opts.profile:
        from pycallgraph import PyCallGraph
        from pycallgraph.output import GraphvizOutput
        with PyCallGraph(output=GraphvizOutput()):
            main(opts)
    else:
        main(opts)

