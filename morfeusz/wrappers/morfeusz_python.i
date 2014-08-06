
//%rename (_Morfeusz) morfeusz::Morfeusz;
//%rename (_ResultsIterator) morfeusz::ResultsIterator;

%rename (_generateByTagId) morfeusz::Morfeusz::generate(const std::string&, int, std::vector<std::string>&);

%extend morfeusz::ResultsIterator {
    morfeusz::ResultsIterator& morfeusz::ResultsIterator::__iter__() {
        return *($self);
    }
};

%extend morfeusz::Morfeusz {
    std::vector<morfeusz::MorphInterpretation> morfeusz::Morfeusz::_generateByTagId(const std::string& lemma, int tagId) const {
        std::vector<morfeusz::MorphInterpretation> res;
        $self->generate(lemma, tagId, res);
        return res;
    }
};

%feature("shadow") morfeusz::ResultsIterator::next %{
def next(self):
    if self.hasNext():
        return $action(self)
    else:
        raise StopIteration
%}

%feature("shadow") morfeusz::Morfeusz::analyseAsIterator(const char*) %{
def analyse_iter(self, text):
    return $action(self, text)
%}

%feature("shadow") morfeusz::Morfeusz::analyse %{
def analyse(self, text):
    res = InterpsList()
    $action(self, text, res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::_generateByTagId %{
def _generateByTagId(self, lemma, tagId):
    res = InterpsList()
    $action(self, lemma, tagId, res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::generate %{
def generate(self, lemma, tagId=None):
    if tagId is not None:
        return self._generateByTagId(lemma, tagId)
    else:
        res = InterpsList()
        $action(self, lemma, res)
        return res
%}

%rename (_orth) morfeusz::MorphInterpretation::orth;

%extend morfeusz::MorphInterpretation {
    %pythoncode %{
        @property
        def orth(self):
            return self._orth.decode('utf8')
        
        @orth.setter
        def orth(self, val):
            self._orth = val.encode('utf8')
    %}
};

%rename (_lemma) morfeusz::MorphInterpretation::lemma;

%extend morfeusz::MorphInterpretation {
    %pythoncode %{
        @property
        def lemma(self):
            return self._lemma.decode('utf8')
        
        @lemma.setter
        def lemma(self, val):
            self._lemma = val.encode('utf8')
    %}
};

%feature("shadow") morfeusz::IdResolver::getTag %{
def getTag(self, tagId):
    return $action(self, tagId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getTagId %{
def getTagId(self, tag):
    return $action(self, tag.encode('utf8'))
%}

%feature("shadow") morfeusz::IdResolver::getName %{
def getName(self, nameId):
    return $action(self, nameId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getNameId %{
def getNameId(self, name):
    return $action(self, name.encode('utf8'))
%}

%feature("shadow") morfeusz::IdResolver::getLabelsAsString %{
def getLabelsAsUnicode(self, labelsId):
    return $action(self, labelsId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getLabels %{
def getLabels(self, labelsId):
    return { l.decode('utf8') for l in $action(self, labelsId) }
%}

%feature("shadow") morfeusz::IdResolver::getLabelsId %{
def getLabelsId(self, labelsStr):
    return $action(self, labelsStr.encode('utf8'))
%}

%include "std_vector.i"
%include "std_string.i"
%include "std_list.i"
%include "std_set.i"
%include "std_except.i"
%include "exception.i"
%include "typemaps.i"
