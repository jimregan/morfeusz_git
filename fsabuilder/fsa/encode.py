'''
Created on Oct 23, 2013

@author: lennyn
'''

class Encoder(object):
    '''
    classdocs
    '''


    def __init__(self, encoding='utf8'):
        '''
        Constructor
        '''
        self.encoding = encoding
    
    def encodeWord(self, word):
        return bytearray(word, self.encoding)
    
    def encodeData(self, data):
        return bytearray(u'|'.join(data).encode(self.encoding)) + bytearray([0])
    
    def decodeData(self, rawData):
#         print unicode(str(rawData), self.encoding)[:-1]
#         print unicode(str(rawData), self.encoding)[:-1].split(u'|')
        return unicode(str(rawData), self.encoding)[:-1].split(u'|')
    
    def word2SortKey(self, word):
        return word.encode(self.encoding)
