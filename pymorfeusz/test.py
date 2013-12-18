#-*- coding: utf-8 -*-

import morfeusz

print 'Hello world in python!'

morf = morfeusz.Morfeusz()
res = morf.analyze('dupa')
for x in res:
    print x.getStartNode()
print res
