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
from serializer import SimpleSerializerWithStringValues
from visualizer import Visualizer
from optparse import OptionParser

logging.basicConfig(level=logging.DEBUG)

class OutputFormat():
    BINARY = 'BINARY'
    CPP = 'CPP'

class InputFormat():
    ENCODED = 'ENCODED'
    POLIMORF = 'POLIMORF'
    PLAIN = 'PLAIN'

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
    parser.add_option('--input-format',
                        dest='inputFormat',
                        help='input format - ENCODED or POLIMORF')
    parser.add_option('--output-format',
                        dest='outputFormat',
                        help='output format - BINARY or CPP')
    parser.add_option('--visualize',
                        dest='visualize',
                        action='store_true', 
                        default=False,
                        help='visualize result')
    
    opts, args = parser.parse_args()
    
    if None in [opts.inputFile, opts.outputFile, opts.outputFormat, opts.inputFormat]:
        parser.print_help()
        exit(1)
    if not opts.outputFormat.upper() in [OutputFormat.BINARY, OutputFormat.CPP]:
        print >> sys.stderr, 'output format must be one of ('+str([OutputFormat.BINARY, OutputFormat.CPP])+')'
        exit(1)
    if not opts.inputFormat.upper() in [InputFormat.ENCODED, InputFormat.POLIMORF, InputFormat.PLAIN]:
        print >> sys.stderr, 'input format must be one of ('+str([InputFormat.ENCODED, InputFormat.POLIMORF])+')'
        exit(1)
    return opts

def readEncodedInput(inputFile):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for line in f.readlines():
            word, interps = line.strip().split()
            yield word, interps.split(u'|')

def readPolimorfInput(inputFile, encoder):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for entry in convertinput.convertPolimorf(f.readlines(), lambda (word, interp): encoder.word2SortKey(word)):
            yield entry

def readPlainInput(inputFile, encoder):
    with codecs.open(inputFile, 'r', 'utf8') as f:
        for line in sorted(f.readlines(), key=encoder.word2SortKey):
            word = line.strip()
            yield word, ''

if __name__ == '__main__':
    opts = parseOptions()
    encoder = encode.Encoder()
    fsa = FSA(encoder)
    serializer = SimpleSerializerWithStringValues()
    
    inputData = {
                 InputFormat.ENCODED: readEncodedInput(opts.inputFile),
                 InputFormat.POLIMORF: readPolimorfInput(opts.inputFile, encoder),
                 InputFormat.PLAIN: readPlainInput(opts.inputFile, encoder)
                 }[opts.inputFormat]
    
    logging.info('feeding FSA with data ...')
    fsa.feed(inputData)
    logging.info('states num: '+str(fsa.getStatesNum()))
    if opts.outputFormat == 'CPP':
        serializer.serialize2CppFile(fsa, opts.outputFile)
    else:
        serializer.serialize2BinaryFile(fsa, opts.outputFile)
    if opts.visualize:
        Visualizer().visualize(fsa)
