'''
Created on Oct 23, 2013

@author: mlenart
'''
import logging
from common import Interpretation
from morfeuszbuilder.fsa.common import Interpretation4Generator

tag2typenum = {
    'aglt:sg:pri:imperf:nwok': 12,
    'aglt:sg:pri:imperf:wok': 12,
    'aglt:sg:sec:imperf:nwok': 12,
    'aglt:sg:sec:imperf:wok': 12,
    'aglt:pl:pri:imperf:nwok': 13,
    'aglt:pl:pri:imperf:wok': 13,
    'aglt:pl:sec:imperf:nwok': 13,
    'aglt:pl:sec:imperf:wok': 13,
    'praet:sg:m1.m2.m3:imperf:agl': 7,
    'praet:sg:m1.m2.m3:imperf.perf:agl': 7,
    'praet:sg:m1.m2.m3:perf:agl': 7,
    'praet:sg:m1.m2.m3:imperf:nagl': 16,
    'praet:sg:m1.m2.m3:imperf.perf:nagl': 16,
    'praet:sg:m1.m2.m3:perf:nagl': 16,
    'praet:sg:f:imperf': 20,
    'praet:sg:f:imperf.perf': 20,
    'praet:sg:f:perf': 20,
    'praet:sg:m1.m2.m3:imperf': 20,
    'praet:sg:m1.m2.m3:imperf.perf': 20,
    'praet:sg:m1.m2.m3:perf': 20,
    'praet:sg:n1.n2:imperf': 20,
    'praet:sg:n1.n2:imperf.perf': 20,
    'praet:sg:n1.n2:perf': 20,
    'praet:pl:m1.p1:imperf': 21,
    'praet:pl:m1.p1:imperf.perf': 21,
    'praet:pl:m1.p1:perf': 21,
    'praet:pl:m2.m3.f.n1.n2.p2.p3:imperf': 21,
    'praet:pl:m2.m3.f.n1.n2.p2.p3:imperf.perf': 21,
    'praet:pl:m2.m3.f.n1.n2.p2.p3:perf': 21,
    'naj': 10,
    'nie': 5,
    'adj:pl:acc:m1.p1:pos': 1,
    'adj:pl:acc:m2.m3.f.n1.n2.p2.p3:pos': 1,
    'adj:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:pos': 1,
    'adj:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:pos': 1,
    'adj:pl:gen:m1.m2.m3.f.n1.n2.p1.p2.p3:pos': 1,
    'adj:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:pos': 1,
    'adj:pl:loc:m1.m2.m3.f.n1.n2.p1.p2.p3:pos': 1,
    'adj:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:pos': 1,
    'adj:pl:nom.voc:m1.p1:pos': 1,
    'adj:pl:nom.voc:m2.m3.f.n1.n2.p2.p3:pos': 1,
    'adj:sg:acc:m1.m2:pos': 1,
    'adj:sg:acc:n1.n2:pos': 1,
    'adj:sg:dat:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:gen:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:inst:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:loc:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:nom.voc:m1.m2.m3:pos': 1,
    'adj:sg:nom.voc:m1.m2.m3:pos|adj:sg:acc:m3:pos': 1,
    'adj:sg:nom.voc:n1.n2:pos': 1,
    'adj:sg:acc:f:pos': 1,
    'adj:sg:acc.inst:f:pos': 1,
    'adj:sg:acc:m1.m2:pos': 1,
    'adj:sg:acc:m3:pos': 1,
    'adj:sg:dat:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:gen.dat.loc:f:pos': 1,
    'adj:sg:gen:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:inst.loc:m1.m2.m3.n1.n2:pos': 1,
    'adj:sg:nom.voc.acc:n1.n2:pos': 1,
    'adj:sg:nom.voc:f:pos': 1,
    'adj:sg:nom.voc:m1.m2.m3:pos': 1,
    'adj:pl:acc:f:pos': 1,
    'adj:pl:acc:m1:pos': 1,
    'adj:pl:acc:m2:pos': 1,
    'adj:pl:acc:m3:pos': 1,
    'adj:pl:acc:n1:pos': 1,
    'adj:pl:acc:n2:pos': 1,
    'adj:pl:acc:p1:pos': 1,
    'adj:pl:acc:p2:pos': 1,
    'adj:pl:acc:p3:pos': 1,
    'adj:pl:dat:f:pos': 1,
    'adj:pl:dat:m1:pos': 1,
    'adj:pl:dat:m2:pos': 1,
    'adj:pl:dat:m3:pos': 1,
    'adj:pl:dat:n1:pos': 1,
    'adj:pl:dat:n2:pos': 1,
    'adj:pl:dat:p1:pos': 1,
    'adj:pl:dat:p2:pos': 1,
    'adj:pl:dat:p3:pos': 1,
    'adj:pl:gen:f:pos': 1,
    'adj:pl:gen:m1:pos': 1,
    'adj:pl:gen:m2:pos': 1,
    'adj:pl:gen:m3:pos': 1,
    'adj:pl:gen:n1:pos': 1,
    'adj:pl:gen:n2:pos': 1,
    'adj:pl:gen:p1:pos': 1,
    'adj:pl:gen:p2:pos': 1,
    'adj:pl:gen:p3:pos': 1,
    'adj:pl:inst:f:pos': 1,
    'adj:pl:inst:m1:pos': 1,
    'adj:pl:inst:m2:pos': 1,
    'adj:pl:inst:m3:pos': 1,
    'adj:pl:inst:n1:pos': 1,
    'adj:pl:inst:n2:pos': 1,
    'adj:pl:inst:p1:pos': 1,
    'adj:pl:inst:p2:pos': 1,
    'adj:pl:inst:p3:pos': 1,
    'adj:pl:loc:f:pos': 1,
    'adj:pl:loc:m1:pos': 1,
    'adj:pl:loc:m2:pos': 1,
    'adj:pl:loc:m3:pos': 1,
    'adj:pl:loc:n1:pos': 1,
    'adj:pl:loc:n2:pos': 1,
    'adj:pl:loc:p1:pos': 1,
    'adj:pl:loc:p2:pos': 1,
    'adj:pl:loc:p3:pos': 1,
    'adj:pl:nom:f:pos': 1,
    'adj:pl:nom:m1:pos': 1,
    'adj:pl:nom:m2:pos': 1,
    'adj:pl:nom:m3:pos': 1,
    'adj:pl:nom:n1:pos': 1,
    'adj:pl:nom:n2:pos': 1,
    'adj:pl:nom:p1:pos': 1,
    'adj:pl:nom:p2:pos': 1,
    'adj:pl:nom:p3:pos': 1,
    'adj:sg:acc:f:pos': 1,
    'adj:sg:acc:m1:pos': 1,
    'adj:sg:acc:m2:pos': 1,
    'adj:sg:acc:m3:pos': 1,
    'adj:sg:acc:n1:pos': 1,
    'adj:sg:acc:n2:pos': 1,
    'adj:sg:dat:f:pos': 1,
    'adj:sg:dat:m1:pos': 1,
    'adj:sg:dat:m2:pos': 1,
    'adj:sg:dat:m3:pos': 1,
    'adj:sg:dat:n1:pos': 1,
    'adj:sg:dat:n2:pos': 1,
    'adj:sg:gen:f:pos': 1,
    'adj:sg:gen:m1:pos': 1,
    'adj:sg:gen:m2:pos': 1,
    'adj:sg:gen:m3:pos': 1,
    'adj:sg:gen:n1:pos': 1,
    'adj:sg:gen:n2:pos': 1,
    'adj:sg:inst:f:pos': 1,
    'adj:sg:inst:m1:pos': 1,
    'adj:sg:inst:m2:pos': 1,
    'adj:sg:inst:m3:pos': 1,
    'adj:sg:inst:n1:pos': 1,
    'adj:sg:inst:n2:pos': 1,
    'adj:sg:loc:f:pos': 1,
    'adj:sg:loc:m1:pos': 1,
    'adj:sg:loc:m2:pos': 1,
    'adj:sg:loc:m3:pos': 1,
    'adj:sg:loc:n1:pos': 1,
    'adj:sg:loc:n2:pos': 1,
    'adj:sg:nom:f:pos': 1,
    'adj:sg:nom:m1:pos': 1,
    'adj:sg:nom:m2:pos': 1,
    'adj:sg:nom:m3:pos': 1,
    'adj:sg:nom:n1:pos': 1,
    'adj:sg:nom:n2:pos': 1,
    'adj:pl:acc:m1.p1:sup': 19,
    'adj:pl:acc:m2.m3.f.n1.n2.p2.p3:sup': 19,
    'adj:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:sup': 19,
    'adj:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:sup': 19,
    'adj:pl:gen:m1.m2.m3.f.n1.n2.p1.p2.p3:sup': 19,
    'adj:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:sup': 19,
    'adj:pl:loc:m1.m2.m3.f.n1.n2.p1.p2.p3:sup': 19,
    'adj:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:sup': 19,
    'adj:pl:nom.voc:m1.p1:sup': 19,
    'adj:pl:nom.voc:m2.m3.f.n1.n2.p2.p3:sup': 19,
    'adj:sg:acc:f:sup': 19,
    'adj:sg:acc.inst:f:sup': 19,
    'adj:sg:acc:m1.m2:sup': 19,
    'adj:sg:acc:m3:sup': 19,
    'adj:sg:acc:n1.n2:sup': 19,
    'adj:sg:dat:f:sup': 19,
    'adj:sg:dat:m1.m2.m3.n1.n2:sup': 19,
    'adj:sg:gen:f:sup': 19,
    'adj:sg:gen.dat.loc:f:sup': 19,
    'adj:sg:gen:m1.m2.m3.n1.n2:sup': 19,
    'adj:sg:inst:f:sup': 19,
    'adj:sg:inst:m1.m2.m3.n1.n2:sup': 19,
    'adj:sg:inst.loc:m1.m2.m3.n1.n2:sup': 19,
    'adj:sg:loc:f:sup': 19,
    'adj:sg:loc:m1.m2.m3.n1.n2:sup': 19,
    'adj:sg:nom.acc:n1.n2:sup': 19,
    'adj:sg:nom.voc:f:sup': 19,
    'adj:sg:nom.voc:m1.m2.m3:sup': 19,
    'adj:sg:nom.voc:n1.n2:sup': 19,
    'adj:pl:acc:f:sup': 19,
    'adj:pl:acc:m1:sup': 19,
    'adj:pl:acc:m2:sup': 19,
    'adj:pl:acc:m3:sup': 19,
    'adj:pl:acc:n1:sup': 19,
    'adj:pl:acc:n2:sup': 19,
    'adj:pl:acc:p1:sup': 19,
    'adj:pl:acc:p2:sup': 19,
    'adj:pl:acc:p3:sup': 19,
    'adj:pl:dat:f:sup': 19,
    'adj:pl:dat:m1:sup': 19,
    'adj:pl:dat:m2:sup': 19,
    'adj:pl:dat:m3:sup': 19,
    'adj:pl:dat:n1:sup': 19,
    'adj:pl:dat:n2:sup': 19,
    'adj:pl:dat:p1:sup': 19,
    'adj:pl:dat:p2:sup': 19,
    'adj:pl:dat:p3:sup': 19,
    'adj:pl:gen:f:sup': 19,
    'adj:pl:gen:m1:sup': 19,
    'adj:pl:gen:m2:sup': 19,
    'adj:pl:gen:m3:sup': 19,
    'adj:pl:gen:n1:sup': 19,
    'adj:pl:gen:n2:sup': 19,
    'adj:pl:gen:p1:sup': 19,
    'adj:pl:gen:p2:sup': 19,
    'adj:pl:gen:p3:sup': 19,
    'adj:pl:inst:f:sup': 19,
    'adj:pl:inst:m1:sup': 19,
    'adj:pl:inst:m2:sup': 19,
    'adj:pl:inst:m3:sup': 19,
    'adj:pl:inst:n1:sup': 19,
    'adj:pl:inst:n2:sup': 19,
    'adj:pl:inst:p1:sup': 19,
    'adj:pl:inst:p2:sup': 19,
    'adj:pl:inst:p3:sup': 19,
    'adj:pl:loc:f:sup': 19,
    'adj:pl:loc:m1:sup': 19,
    'adj:pl:loc:m2:sup': 19,
    'adj:pl:loc:m3:sup': 19,
    'adj:pl:loc:n1:sup': 19,
    'adj:pl:loc:n2:sup': 19,
    'adj:pl:loc:p1:sup': 19,
    'adj:pl:loc:p2:sup': 19,
    'adj:pl:loc:p3:sup': 19,
    'adj:pl:nom:f:sup': 19,
    'adj:pl:nom:m1:sup': 19,
    'adj:pl:nom:m2:sup': 19,
    'adj:pl:nom:m3:sup': 19,
    'adj:pl:nom:n1:sup': 19,
    'adj:pl:nom:n2:sup': 19,
    'adj:pl:nom:p1:sup': 19,
    'adj:pl:nom:p2:sup': 19,
    'adj:pl:nom:p3:sup': 19,
    'adj:sg:acc:f:sup': 19,
    'adj:sg:acc:m1:sup': 19,
    'adj:sg:acc:m2:sup': 19,
    'adj:sg:acc:m3:sup': 19,
    'adj:sg:acc:n1:sup': 19,
    'adj:sg:acc:n2:sup': 19,
    'adj:sg:dat:f:sup': 19,
    'adj:sg:dat:m1:sup': 19,
    'adj:sg:dat:m2:sup': 19,
    'adj:sg:dat:m3:sup': 19,
    'adj:sg:dat:n1:sup': 19,
    'adj:sg:dat:n2:sup': 19,
    'adj:sg:gen:f:sup': 19,
    'adj:sg:gen:m1:sup': 19,
    'adj:sg:gen:m2:sup': 19,
    'adj:sg:gen:m3:sup': 19,
    'adj:sg:gen:n1:sup': 19,
    'adj:sg:gen:n2:sup': 19,
    'adj:sg:inst:f:sup': 19,
    'adj:sg:inst:m1:sup': 19,
    'adj:sg:inst:m2:sup': 19,
    'adj:sg:inst:m3:sup': 19,
    'adj:sg:inst:n1:sup': 19,
    'adj:sg:inst:n2:sup': 19,
    'adj:sg:loc:f:sup': 19,
    'adj:sg:loc:m1:sup': 19,
    'adj:sg:loc:m2:sup': 19,
    'adj:sg:loc:m3:sup': 19,
    'adj:sg:loc:n1:sup': 19,
    'adj:sg:loc:n2:sup': 19,
    'adj:sg:nom:f:sup': 19,
    'adj:sg:nom:m1:sup': 19,
    'adj:sg:nom:m2:sup': 19,
    'adj:sg:nom:m3:sup': 19,
    'adj:sg:nom:n1:sup': 19,
    'adj:sg:nom:n2:sup': 19,
    'adv:sup': 19,
    'winien:sg:m1.m2.m3:imperf': 3,
    'winien:sg:f:imperf': 3,
    'winien:sg:n1.n2:imperf': 3,
    'winien:pl:m1.p1:imperf': 3,
    'winien:pl:m2.m3.f.n1.n2.p2.p3:imperf': 3,
    'adja': 15,
    'ger:sg:dat.loc:n2:imperf:neg': 18,
    'ger:sg:dat.loc:n2:imperf.perf:neg': 18,
    'ger:sg:dat.loc:n2:perf:neg': 18,
    'ger:sg:gen:n2:imperf:neg': 18,
    'ger:sg:gen:n2:imperf.perf:neg': 18,
    'ger:sg:gen:n2:perf:neg': 18,
    'ger:sg:inst:n2:imperf:neg': 18,
    'ger:sg:inst:n2:imperf.perf:neg': 18,
    'ger:sg:inst:n2:perf:neg': 18,
    'ger:sg:nom.acc:n2:imperf:neg': 18,
    'ger:sg:nom.acc:n2:imperf.perf:neg': 18,
    'ger:sg:nom.acc:n2:perf:neg': 18,
    'pact:pl:acc:m1.p1:imperf:neg': 18,
    'pact:pl:acc:m1.p1:imperf.perf:neg': 18,
    'pact:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'pact:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'pact:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'pact:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'pact:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'pact:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'pact:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:imperf:neg': 18,
    'pact:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:imperf.perf:neg': 18,
    'pact:pl:nom:m1.p1:imperf:neg': 18,
    'pact:pl:nom:m1.p1:imperf.perf:neg': 18,
    'pact:sg:acc.inst:f:imperf:neg': 18,
    'pact:sg:acc.inst:f:imperf.perf:neg': 18,
    'pact:sg:acc:m1.m2:imperf:neg': 18,
    'pact:sg:acc:m1.m2:imperf.perf:neg': 18,
    'pact:sg:acc:m3:imperf:neg': 18,
    'pact:sg:acc:m3:imperf.perf:neg': 18,
    'pact:sg:dat:m1.m2.m3.n1.n2:imperf:neg': 18,
    'pact:sg:dat:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'pact:sg:gen.dat.loc:f:imperf:neg': 18,
    'pact:sg:gen.dat.loc:f:imperf.perf:neg': 18,
    'pact:sg:gen:m1.m2.m3.n1.n2:imperf:neg': 18,
    'pact:sg:gen:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'pact:sg:inst.loc:m1.m2.m3.n1.n2:imperf:neg': 18,
    'pact:sg:inst.loc:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'pact:sg:nom.acc:n1.n2:imperf:neg': 18,
    'pact:sg:nom.acc:n1.n2:imperf.perf:neg': 18,
    'pact:sg:nom:f:imperf:neg': 18,
    'pact:sg:nom:f:imperf.perf:neg': 18,
    'pact:sg:nom:m1.m2.m3:imperf:neg': 18,
    'pact:sg:nom:m1.m2.m3:imperf.perf:neg': 18,
    'ppas:pl:acc:m1.p1:imperf:neg': 18,
    'ppas:pl:acc:m1.p1:imperf.perf:neg': 18,
    'ppas:pl:acc:m1.p1:perf:neg': 18,
    'ppas:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'ppas:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'ppas:pl:dat:m1.m2.m3.f.n1.n2.p1.p2.p3:perf:neg': 18,
    'ppas:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'ppas:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'ppas:pl:gen.loc:m1.m2.m3.f.n1.n2.p1.p2.p3:perf:neg': 18,
    'ppas:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf:neg': 18,
    'ppas:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:imperf.perf:neg': 18,
    'ppas:pl:inst:m1.m2.m3.f.n1.n2.p1.p2.p3:perf:neg': 18,
    'ppas:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:imperf:neg': 18,
    'ppas:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:imperf.perf:neg': 18,
    'ppas:pl:nom.acc:m2.m3.f.n1.n2.p2.p3:perf:neg': 18,
    'ppas:pl:nom:m1.p1:imperf:neg': 18,
    'ppas:pl:nom:m1.p1:imperf.perf:neg': 18,
    'ppas:pl:nom:m1.p1:perf:neg': 18,
    'ppas:sg:acc.inst:f:imperf:neg': 18,
    'ppas:sg:acc.inst:f:imperf.perf:neg': 18,
    'ppas:sg:acc.inst:f:perf:neg': 18,
    'ppas:sg:acc:m1.m2:imperf:neg': 18,
    'ppas:sg:acc:m1.m2:imperf.perf:neg': 18,
    'ppas:sg:acc:m1.m2:perf:neg': 18,
    'ppas:sg:acc:m3:imperf:neg': 18,
    'ppas:sg:acc:m3:imperf.perf:neg': 18,
    'ppas:sg:acc:m3:perf:neg': 18,
    'ppas:sg:dat:m1.m2.m3.n1.n2:imperf:neg': 18,
    'ppas:sg:dat:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'ppas:sg:dat:m1.m2.m3.n1.n2:perf:neg': 18,
    'ppas:sg:gen.dat.loc:f:imperf:neg': 18,
    'ppas:sg:gen.dat.loc:f:imperf.perf:neg': 18,
    'ppas:sg:gen.dat.loc:f:perf:neg': 18,
    'ppas:sg:gen:m1.m2.m3.n1.n2:imperf:neg': 18,
    'ppas:sg:gen:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'ppas:sg:gen:m1.m2.m3.n1.n2:perf:neg': 18,
    'ppas:sg:inst.loc:m1.m2.m3.n1.n2:imperf:neg': 18,
    'ppas:sg:inst.loc:m1.m2.m3.n1.n2:imperf.perf:neg': 18,
    'ppas:sg:inst.loc:m1.m2.m3.n1.n2:perf:neg': 18,
    'ppas:sg:nom.acc:n1.n2:imperf:neg': 18,
    'ppas:sg:nom.acc:n1.n2:imperf.perf:neg': 18,
    'ppas:sg:nom.acc:n1.n2:perf:neg': 18,
    'ppas:sg:nom:f:imperf:neg': 18,
    'ppas:sg:nom:f:imperf.perf:neg': 18,
    'ppas:sg:nom:f:perf:neg': 18,
    'ppas:sg:nom:m1.m2.m3:imperf:neg': 18,
    'ppas:sg:nom:m1.m2.m3:imperf.perf:neg': 18,
    'ppas:sg:nom:m1.m2.m3:perf:neg': 18,
    'ppron3:sg:gen.acc:m1.m2.m3:ter:nakc:praep': 8,
    'prep:acc': 6,
    'prep:acc:wok': 6,
    'prep:acc.inst': 6,
    'prep:acc.inst:wok': 6,
    'prep:inst.acc': 6,
    'prep:inst.acc:wok': 6,
    'prep:inst.gen.acc:wok': 6,
    'prep:acc.loc': 6,
    'prep:acc.loc:wok': 6,
    'prep:loc.acc': 6,
    'prep:loc.acc:wok': 6,
    'prep:gen': 6,
    'prep:gen.dat': 6,
    'prep:gen:wok': 6,
    'prep:gen.inst:wok': 6,
    'brev:pun': 9,
    'brev:npun': 9,
    'intrj': 9,
    'burk': 9,
}

def _mergeEntries(inputLines):
    prevKey = None
    prevInterps = None
    for key, interp in inputLines:
        key = key.lower()
        assert key
        if prevKey and prevKey == key:
            prevInterps.append(interp)
        else:
            if prevKey:
                yield (prevKey, frozenset(prevInterps))
            prevKey = key
            prevInterps = [interp]
    yield (prevKey, frozenset(prevInterps))

class PolimorfConverter4Analyzer(object):
    
    def __init__(self, tagset, encoder, inputEncoding='utf8'):
        self.tagset = tagset
        self.encoder = encoder
        self.inputEncoding = inputEncoding

    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            orth, base, tag, name = line.split(u'\t')
            tagnum = self.tagset.tag2tagnum[tag]
            namenum = self.tagset.name2namenum[name]
            typenum = tag2typenum.get(tag, 0)
            yield '%s %s %d %d %d' % (
                                      orth.encode(self.inputEncoding), 
                                      base.encode(self.inputEncoding), 
                                      tagnum, namenum, typenum)
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split(' ')[0].decode('utf8')))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
    #             print line
                orth, base, tagnum, namenum, typenum = line.split(u' ')
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                yield (orth, Interpretation(orth, base, tagnum, namenum, typenum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))))

class PolimorfConverter4Generator(object):
    
    def __init__(self, tagset, encoder, inputEncoding='utf8'):
        self.tagset = tagset
        self.encoder = encoder
        self.inputEncoding = inputEncoding
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            orth, base, tag, name = line.split(u'\t')
            tagnum = self.tagset.tag2tagnum[tag]
            namenum = self.tagset.name2namenum[name]
            yield '%s %s %d %d' % (
                                   orth.encode(self.inputEncoding), 
                                   base.encode(self.inputEncoding), 
                                   tagnum, namenum)
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: (self.encoder.word2SortKey(line.split(' ')[1].decode('utf8')), line))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
    #             print line
                orth, base, tagnum, namenum = line.split(u' ')
                tagnum = int(tagnum)
                namenum = int(namenum)
                yield (base, Interpretation4Generator(orth, base, tagnum, namenum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))))
