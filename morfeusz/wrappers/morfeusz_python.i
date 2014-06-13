%pythoncode %{

def analyze(self, text):
  res = InterpsVector()
  _morfeusz2.Morfeusz_analyze(self, text, res)
  return list(res)

Morfeusz.analyze = analyze

def getOrth(self):
  return _morfeusz2.MorphInterpretation_getOrth(self).decode('utf8')

def getLemma(self):
  return _morfeusz2.MorphInterpretation_getLemma(self).decode('utf8')

def getTag(self):
  return _morfeusz2.MorphInterpretation_getTag(self).decode('utf8')

def getName(self):
  return _morfeusz2.MorphInterpretation_getName(self).decode('utf8')

MorphInterpretation.getOrth = getOrth
MorphInterpretation.getLemma = getLemma
MorphInterpretation.getTag = getTag
MorphInterpretation.getName = getName
%}