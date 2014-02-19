'''
Created on 18 lut 2014

@author: mlenart
'''
import unittest
import os
from morfeuszbuilder.segrules import rulesParser
from morfeuszbuilder.tagset import tagset

class Test(unittest.TestCase):
    print 'do test'
    t = tagset.Tagset(os.path.join(os.path.dirname(__file__), 'polimorf.tagset'))
    parser = rulesParser.RulesParser(t)
    parser.parse(os.path.join(os.path.dirname(__file__), 'segmenty.dat'))
    print 'done'

if __name__ == "__main__":
    unittest.main()
#     testParser()