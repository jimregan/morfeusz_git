
%exception {
    try{
        $action
    }
    catch(const std::ios_base::failure& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const morfeusz::FileFormatException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::invalid_argument& e) {
        SWIG_exception(SWIG_ValueError, const_cast<char*>(e.what()));
    }
    catch(const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.what()));
    }
    catch(const std::string& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.c_str()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%exception morfeusz::Morfeusz::setDictionary {
    try{
        $action
    }
    catch(const std::ios_base::failure& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const morfeusz::MorfeuszException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::invalid_argument& e) {
        SWIG_exception(SWIG_ValueError, const_cast<char*>(e.what()));
    }
    catch(const std::string& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.c_str()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%ignore morfeusz::MorfeuszException;
%ignore morfeusz::FileFormatException;

%rename (_generateByTagId) morfeusz::Morfeusz::generate(const std::string&, int, std::vector<std::string>&);

%extend morfeusz::ResultsIterator {
    morfeusz::ResultsIterator& morfeusz::ResultsIterator::__iter__() {
        return *($self);
    }
};

//%ignore morfeusz::Morfeusz::createInstance(morfeusz::MorfeuszUsage);
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

%feature("shadow") morfeusz::Morfeusz::_analyseAsIterator %{

def analyse_iter(self, text):
    """
    Analyse given text and return an iterator over MorphInterpretation objects as a result.
    """
    return $action(self, text)
%}

%feature("shadow") morfeusz::Morfeusz::analyse %{
def analyse(self, text):
    """
    Analyse given text and return a list of MorphInterpretation objects.
    """
    res = InterpsList()
    $action(self, text, res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::setAggl %{
def setAggl(self, optionString):
    """
    Select agglutination rules option
    """
    $action(self, optionString.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::setPraet %{
def setPraet(self, optionString):
    """
    Select past tense segmentation
    """
    $action(self, optionString.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::setCaseHandling %{
def setCaseHandling(self, option):
    """
    Set case handling option (valid options are CONDITIONALLY_CASE_SENSITIVE, STRICTLY_CASE_SENSITIVE, IGNORE_CASE)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setTokenNumbering %{
def setTokenNumbering(self, option):
    """
    Set token numbering option (valid options are SEPARATE_NUMBERING, CONTINUOUS_NUMBERING)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setWhitespaceHandling %{
def setWhitespaceHandling(self, option):
    """
    Set whitespace handling handling option (valid options are SKIP_WHITESPACES, KEEP_WHITESPACES, APPEND_WHITESPACES)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setDictionary %{
def setDictionary(self, dictName):
    """
    Set dictionary to be used by this instance (by name)
    """
    $action(self, dictName.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::createInstance(morfeusz::MorfeuszUsage) %{
@staticmethod
def _createInstance(usage):
    return $action(usage)
%}

%feature("shadow") morfeusz::Morfeusz::createInstance(const std::string&, morfeusz::MorfeuszUsage) %{
@staticmethod
def createInstance(dictName=None, usage=BOTH_ANALYSE_AND_GENERATE):
    """
    Creates new instance of Morfeusz class. Usage may be BOTH_ANALYZE_AND_GENERATE (default), ONLY_ANALYSE and ONLY_GENERATE.
    """
    if dictName is None:
        return Morfeusz._createInstance(usage)
    else:
        return $action(dictName.encode('utf8'), usage)
%}

%feature("shadow") morfeusz::Morfeusz::_generateByTagId %{
def _generateByTagId(self, lemma, tagId):
    res = InterpsList()
    $action(self, lemma, tagId, res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::generate %{
def generate(self, lemma, tagId=None):
    """
    Perform morphological synthesis on given text and return a list of MorphInterpretation objects.
    """
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

%feature("shadow") morfeusz::MorphInterpretation::getTag %{
def getTag(self, morfeusz):
    """
    Returns tag as string.
    """
    return $action(self, morfeusz)
%}

%feature("shadow") morfeusz::MorphInterpretation::getName %{
def getName(self, morfeusz):
    """
    Returns this interpretation named entity as string
    """
    return $action(self, morfeusz)
%}

%feature("shadow") morfeusz::MorphInterpretation::getLabelsAsString %{
def getLabelsAsUnicode(self, morfeusz):
    """
    Returns this interpretation labels as string
    """
    return $action(self, morfeusz).decode('utf8')
%}

%feature("shadow") morfeusz::MorphInterpretation::getLabels %{
def getLabels(self, morfeusz):
    """
    Returns this interpretation labels as a set of strings
    """
    return { l.decode('utf8') for l in $action(self, morfeusz) }
%}

%feature("shadow") morfeusz::MorphInterpretation::createIgn %{
@staticmethod
def createIgn(startNode, endNode, orth, lemma):
    """
    Creates unknown interpretation
    """
    return $action(self, startNode, endNode, orth.encode('utf8'), lemma.encode('utf8'))
%}

%feature("shadow") morfeusz::MorphInterpretation::createWhitespace %{
@staticmethod
def createWhitespace(startNode, endNode, orth):
    """
    Creates whitespace interpretation
    """
    return $action(self, startNode, endNode, orth.encode('utf8'))
%}

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
