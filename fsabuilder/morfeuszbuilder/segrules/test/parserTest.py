'''
Created on 18 lut 2014

@author: mlenart
'''
import unittest
import os
from morfeuszbuilder.segrules import rulesParser
from morfeuszbuilder.tagset import tagset
from morfeuszbuilder.fsa import visualizer, serializer

class Test(unittest.TestCase):
    print 'do test'
    t = tagset.Tagset(os.path.join(os.path.dirname(__file__), 'polimorf.tagset'))
    parser = rulesParser.RulesParser(t)
    fsas = parser.parse(os.path.join(os.path.dirname(__file__), 'segmenty.dat'))
    fsa = fsas[0]
    for s in fsa.dfs():
        s.debug()
    print 'states:', len(list(fsa.dfs()))
    print 'transitions:', fsa.getTransitionsNum()
    visualizer.Visualizer().visualize(fsa, charLabels=False)
    print 'size:', len(serializer.SimpleSerializer(fsa).fsa2bytearray(bytearray()))
    print 'done'

if __name__ == "__main__":
    unittest.main()
#     testParser()