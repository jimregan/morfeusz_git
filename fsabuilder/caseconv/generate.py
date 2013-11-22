'''
Created on Nov 22, 2013

@author: mlenart
'''

import os
import sys
import logging
from optparse import OptionParser

ARRAY_SIZE = 0x4000

def _parseOptions():
    """
    Parses commandline args
    """
    parser = OptionParser()
#     parser.add_option('-i', '--input-file',
#                         dest='inputFile',
#                         metavar='FILE',
#                         help='path to input file (CaseFolding.txt)')
    parser.add_option('-o', '--output-file',
                        dest='outputFile',
                        metavar='FILE',
                        help='path to output C++ source file')
#     parser.add_option('--header-filename', 
#                         dest='headerFilename',
#                         help='name of the C++ header file')
    
    
    opts, args = parser.parse_args()
    
    if None in [opts.outputFile, opts.constName, opts.headerFilename]:
        logging.error('Missing some options')
        parser.print_help()
        exit(1)
    return opts

def _parseCaseFoldingTxtFile(f):
    table = [code for code in range(ARRAY_SIZE)]
    extendedTable = {}
    for line in f:
        line = line.strip()
        if line and not line.startswith('#'):
            split = line.split('; ')
            code = int(split[0], 16)
            if split[1] in 'CS':
                targetCode = int(split[2], 16)
                if code < ARRAY_SIZE:
                    table[code] = targetCode
                else:
                    extendedTable[code] = targetCode
    return table, extendedTable

def _serializeTable(table):
    res = []
    res.append('{')
    for targetCode in table:
        res.append(str(targetCode))
        res.append(',')
    res.append('}')
    return ''.join(res)

def _serializeExtendedTable(table):
    res = []
    res.append('{')
    for code, targetCode in table.iteritems():
        res.append('{')
        res.append(str(code))
        res.append(',')
        res.append(str(targetCode))
        res.append('},')
    res.append('}')
    return ''.join(res)

def _serialize(table, extendedTable):
    return '''
#include "case_folding.hpp"

const unsigned int CASE_FOLDING_TABLE_SIZE = {tableSize};
const unsigned int EXT_CASE_FOLDING_TABLE_SIZE = {extendedTableSize};
const uint32_t CASE_FOLDING_TABLE[] = {table};
const uint32_t EXT_CASE_FOLDING_TABLE[][2] = {extendedTable};
'''.format(
           tableSize=len(table), 
           table=_serializeTable(table),
           extendedTableSize=len(extendedTable),
           extendedTable=_serializeExtendedTable(extendedTable))

if __name__ == '__main__':
    outfile = sys.argv[1]
    with open(os.path.join(os.path.dirname(__file__), 'CaseFolding.txt'), 'r') as f:
        table, extendedTable = _parseCaseFoldingTxtFile(f)
        with open(sys.argv[1], 'w') as f1:
            f1.write(_serialize(table, extendedTable))
