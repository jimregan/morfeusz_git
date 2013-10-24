#-*- coding: utf-8 -*-
'''
Created on Oct 8, 2013

@author: lennyn
'''
import unittest
from fsa import fsa, visualizer, encode

class Test(unittest.TestCase):


    def testSimpleConstruction(self):
        print 'dupa'
        a = fsa.FSA(encode.Encoder())
        input = sorted([
                (u'bić', ''),
                (u'bij', ''),
                (u'biją', ''),
                (u'bijcie', ''),
                (u'bije', ''),
                (u'bijecie', ''),
                (u'bijemy', ''),
                (u'bijesz', ''),
                (u'biję', ''),
                (u'bijmy', ''),
                (u'bili', 'asd'),
                (u'biliby', ''),
                (u'bilibyście', ''),
                (u'bilibyśmy', ''),
                (u'biliście', 'asdfas'),
                (u'biliśmy', ''),
                (u'bił', 'wersadfas'),
                (u'biła', 'asdfasd'),
                (u'biłaby', 'asdfa'),
                (u'biłabym', ''),
                (u'biłabyś', 'asdfa'),
                (u'biłam', 'dfas'),
                (u'biłaś', 'asdfas'),
                (u'biłby', ''),
                (u'biłbym', 'asdfa'),
                (u'biłbyś', ''),
                (u'biłem', ''),
                (u'biłeś', 'sadfa'),
                (u'biły', ''),
                (u'biłyby', ''),
                (u'biłybyście', ''),
                (u'biłybyśmy', ''),
                (u'biłyście', ''),
                (u'biłyśmy', ''),
                ], key=lambda w: bytearray(w[0], 'utf8'))
        a.feed(input)
        print a.getStatesNum()
#         print a.tryToRecognize(u'bi')
#         print a.tryToRecognize(u'bić')
#         print a.tryToRecognize(u'bili')
        for w, res in input:
            print w, res, a.tryToRecognize(w)
            recognized = a.tryToRecognize(w)
            if type(res) in [str, unicode]:
                recognized = recognized[0]
            assert recognized == res
        a.calculateOffsets(lambda state: 1 + 4 * len(state.transitionsMap.keys()) + (len(state.encodedData) if state.isAccepting() else 0))
        visualizer.Visualizer().visualize(a)
        print 'done'

if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testSimpleConstruction']
    unittest.main()