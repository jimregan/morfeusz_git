#!/usr/bin/python
# -*- coding:utf-8 -*-
'''
Created on 21 paź 2013

@author: mlenart
'''
import os
import sys
import logging
import codecs
from morfeuszbuilder.fsa import encode
from morfeuszbuilder.fsa import convertinput
from morfeuszbuilder.fsa.fsa import FSA
from morfeuszbuilder.fsa.serializer import VLengthSerializer1, VLengthSerializer2, SimpleSerializer
from morfeuszbuilder.tagset.tagset import Tagset
from morfeuszbuilder.segrules import rulesParser
from optparse import OptionParser

# class InputFormat():
#     ENCODED = 'ENCODED'
#     POLIMORF = 'POLIMORF'
#     PLAIN = 'PLAIN'

class SerializationMethod():
    SIMPLE = 'SIMPLE'
    V1 = 'V1'
    V2 = 'V2'

def _checkOption(opt, parser, msg):
    if opt is None:
        print >> sys.stderr, msg
        parser.print_help()
        exit(1)

def _checkExactlyOneOptionSet(optsList, parser, msg):
    if optsList.count(True) != 1:
        print >> sys.stderr, msg
        parser.print_help()
        exit(1)

def _parseListCallback(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))

def _checkOpen(filename, mode):
    try:
        with open(filename, mode) as _:
            pass
    except IOError as ex:
        print >> sys.stderr, str(ex)
        exit(1)

def _parseOptions():
    """
    Parses commandline args
    """
    parser = OptionParser()
    parser.add_option('--input-files',
                        type='string',
                        dest='inputFiles',
                        action='callback',
                        callback=_parseListCallback,
                        metavar='FILES',
                        help='comma separated list of files')
    parser.add_option('--tagset-file',
                        dest='tagsetFile',
                        metavar='FILE',
                        help='path to the file with tagset')
    parser.add_option('--segments-file',
                        dest='segmentsFile',
                        metavar='FILE',
                        help='path to the file with segment rules')
    parser.add_option('--trim-supneg',
                        dest='trimSupneg',
                        default=False,
                        action='store_true',
                        help='trim "naj" and "nie" prefixes from words tagged as "%:sup" and "%:neg" respectively. Valid only for analysis.')
    parser.add_option('-o', '--output-file',
                        dest='outputFile',
                        metavar='FILE',
                        help='path to output file')
    parser.add_option('-a', '--analyzer',
                        dest='analyzer',
                        action='store_true',
                        default=False,
                        help='Generate FSA for morphological analysis')
    parser.add_option('-g', '--generator',
                        dest='generator',
                        action='store_true',
                        default=False,
                        help='Generate FSA for morphological synthesis')
    parser.add_option('--cpp',
                        dest='cpp',
                        action='store_true',
                        default=False,
                        help='Encode binary data in c++ file')
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
    #~ parser.add_option('--visualize',
                        #~ dest='visualize',
                        #~ action='store_true', 
                        #~ default=False,
                        #~ help='visualize result')
    parser.add_option('--train-file',
                        dest='trainFile',
                        help='A text file used for training. Should contain words from some large corpus - one word in each line')
    parser.add_option('--debug',
                        dest='debug',
                        action='store_true',
                        default=False,
                        help='output some debugging info')
    #~ parser.add_option('--profile',
                        #~ dest='profile',
                        #~ action='store_true',
                        #~ default=False,
                        #~ help='show profiling graph (required pycallgraph and graphviz')
    
    opts, args = parser.parse_args()
    
    _checkOption(opts.inputFiles, parser, "Input file is missing")
    _checkOption(opts.outputFile, parser, "Output file is missing")
    _checkOption(opts.tagsetFile, parser, "Tagset file is missing")
    _checkOption(opts.serializationMethod, parser, "Serialization method file is missing")
    _checkExactlyOneOptionSet([opts.analyzer, opts.generator], 
                              parser, 'Must set exactly one FSA type: --analyzer or --generator')
    
    _checkOpen(opts.tagsetFile, 'r')
    for filename in opts.inputFiles:
        _checkOpen(filename, 'r')
    _checkOpen(opts.outputFile, 'w')
    _checkOption(opts.segmentsFile, parser, "Segment rules file is missing")
    if opts.analyzer:
        _checkOpen(opts.segmentsFile, 'r')
    
    if not opts.serializationMethod.upper() in [SerializationMethod.SIMPLE, SerializationMethod.V1, SerializationMethod.V2]:
        print >> sys.stderr, '--serialization-method must be one of ('+str([SerializationMethod.SIMPLE, SerializationMethod.V1, SerializationMethod.V2])+')'
        parser.print_help()
        exit(1)
    
    return opts

def _concatFiles(inputFiles):
#     return open(inputFiles[0], 'r')
    for inputFile in inputFiles:
        if inputFile:
            with open(inputFile, 'r') as f:
                for line in f:
                    yield line

def _readPolimorfInput4Analyzer(inputFiles, tagset, encoder, segmentRulesManager, trimSupneg):
    logging.info('reading analyzer data from %s', str(inputFiles))
    for entry in convertinput.PolimorfConverter4Analyzer(tagset, encoder, 'utf8', segmentRulesManager, trimSupneg).convert(_concatFiles(inputFiles)):
        yield entry

def _readPolimorfInput4Generator(inputFiles, tagset, encoder, segmentRulesManager):
    logging.info('reading generator data from %s', str(inputFiles))
    for entry in convertinput.PolimorfConverter4Generator(tagset, encoder, 'utf8', segmentRulesManager).convert(_concatFiles(inputFiles)):
        yield entry

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

def buildAnalyzerFromPoliMorf(inputFiles, tagset, segmentRulesManager, trimSupneg):
    encoder = encode.MorphEncoder()
    fsa = FSA(encoder, tagset)
    for word, data in _readPolimorfInput4Analyzer(inputFiles, tagset, encoder, segmentRulesManager, trimSupneg):
#         print word, data
        fsa.addEntry(word, data)
    fsa.close()
    logging.info('------')
    logging.info('Analyzer FSA stats:')
    logging.info('------')
    _printStats(fsa)
    return fsa

def buildGeneratorFromPoliMorf(inputFiles, tagset, segmentRulesManager):
    encoder = encode.Encoder4Generator()
    fsa = FSA(encoder, tagset)
    inputData = _readPolimorfInput4Generator(inputFiles, tagset, encoder, segmentRulesManager)
    for word, data in inputData:
        fsa.addEntry(word, data)
    fsa.close()
    logging.info('------')
    logging.info('Generator FSA stats:')
    logging.info('------')
    _printStats(fsa)
    return fsa

def main(opts):
    if opts.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    
    if opts.analyzer:
        logging.info('*** building analyzer ***')
    else:
        logging.info('*** building generator ***')
    
    logging.info('reading tagset from %s', opts.tagsetFile)
    tagset = Tagset(opts.tagsetFile)
    rulesType = rulesParser.RulesParser.PARSE4ANALYZER if opts.analyzer else rulesParser.RulesParser.PARSE4GENERATOR
    segmentRulesManager = rulesParser.RulesParser(tagset, rulesType).parse(opts.segmentsFile)
    segmentationRulesData = segmentRulesManager.serialize()
    
    if opts.analyzer:
        fsa = buildAnalyzerFromPoliMorf(opts.inputFiles, tagset, segmentRulesManager, opts.trimSupneg)
    else:
        fsa = buildGeneratorFromPoliMorf(opts.inputFiles, tagset, segmentRulesManager)
    
    if opts.trainFile:
        logging.info('training with '+opts.trainFile+' ...')
        fsa.train(_readTrainData(opts.trainFile))
        logging.info('done training')
        
    serializer = {
                  SerializationMethod.SIMPLE: SimpleSerializer,
                  SerializationMethod.V1: VLengthSerializer1,
                  SerializationMethod.V2: VLengthSerializer2,
                  }[opts.serializationMethod](fsa)
    
    if opts.cpp:
        serializer.serialize2CppFile(opts.outputFile, generator=opts.generator, segmentationRulesData=segmentationRulesData)
    else:
        serializer.serialize2BinaryFile(opts.outputFile, segmentationRulesData=segmentationRulesData)
    
    logging.info('total FSA size (in bytes): '+str(fsa.initialState.reverseOffset))
#     {
#      OutputFormat.CPP: serializer.serialize2CppFile,
#      OutputFormat.BINARY: serializer.serialize2BinaryFile
#      }[opts.outputFormat](opts.outputFile)

if __name__ == '__main__':
    import os
    opts = _parseOptions()
    try:
        main(opts)
#     except Exception as ex:
#         print >> sys.stderr, unicode(ex).encode('utf8')
#         sys.exit(1)
    finally:
        pass

