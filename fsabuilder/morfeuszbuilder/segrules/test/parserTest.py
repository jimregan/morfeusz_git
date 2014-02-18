'''
Created on 18 lut 2014

@author: mlenart
'''
import unittest
import os
from morfeuszbuilder.segrules import rulesParser
from morfeuszbuilder.tagset import tagset

class Test(unittest.TestCase):
    t = tagset.Tagset(os.path.join(os.path.dirname(__file__), 'polimorf.tagset'))
    parser = rulesParser.RulesParser(t)
    parser.parse(os.path.join(os.path.dirname(__file__), 'segmenty.dat'))

if __name__ == "__main__":
    unittest.main()
#     testParser()