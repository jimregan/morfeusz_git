'''
Created on Feb 19, 2014

@author: lennyn
'''

class FSABuilderException(Exception):
    '''
    Exception in configFile module
    '''
    
    def __init__(self, msg):
        self.msg = msg
    
    def __str__(self):
        return 'Failed to create Morfeusz files: ' + self.msg

class SegtypesException(FSABuilderException):
    
    def __init__(self, msg):
        self.msg = msg
    
    def __str__(self):
        return u'Error in segment rules: %s' % self.msg

class ConfigFileException(FSABuilderException):
    
    def __init__(self, filename, lineNum, msg):
        self.filename = filename
        self.lineNum = lineNum
        self.msg = msg
    
    def __str__(self):
        return u'%s:%d - %s' % (self.filename, self.lineNum, self.msg)
