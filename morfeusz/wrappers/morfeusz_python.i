%pythoncode %{

def _analyze(self, text):
  res = InterpsVector()
  _morfeusz2.Morfeusz_analyze(self, text.encode('utf8'), res)
  return list(res)

Morfeusz.analyze = _analyze

def _generate(self, text):
  res = InterpsVector()
  _morfeusz2.Morfeusz_generate(self, text.encode('utf8'), res)
  return list(res)

Morfeusz.generate = _generate

def _getOrth(self):
  return _morfeusz2.MorphInterpretation_getOrth(self).decode('utf8')

def _getLemma(self):
  return _morfeusz2.MorphInterpretation_getLemma(self).decode('utf8')

def _getTag(self):
  return _morfeusz2.MorphInterpretation_getTag(self).decode('utf8')

def _getName(self):
  return _morfeusz2.MorphInterpretation_getName(self).decode('utf8')

MorphInterpretation.getOrth = _getOrth
MorphInterpretation.getLemma = _getLemma
MorphInterpretation.getTag = _getTag
MorphInterpretation.getName = _getName
%}