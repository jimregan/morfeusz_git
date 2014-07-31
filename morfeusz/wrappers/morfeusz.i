
// XXX
// set module name with preprocessor
// because Mac OS X doesn't seem to recognize jniclassname option
#ifdef SWIGJAVA
%module(allprotected="1") MorfeuszWrapper
#else
%module(allprotected="1") morfeusz2
#endif

%feature("autodoc", "2");
%{
#include "morfeusz2.h"
#include "MorfeuszInternal.hpp"
#include <vector>
%}

#ifdef SWIGJAVA
%include "morfeusz_java.i"
#endif

%include "std_vector.i"
%include "std_string.i"
%include "std_except.i"
%include "exception.i"
%include "typemaps.i"

%exception {
    try{
        $action
    }
    catch(const morfeusz::FileFormatException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.what()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

namespace morfeusz {
    
    %ignore MorphInterpretation::MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const std::string& lemma,
            int tagnum,
            int namenum,
            const std::vector<std::string>* qualifiers,
            const Tagset<std::string>* tagset);

    %ignore MorphInterpretation::createIgn(int startNode, int endNode, const std::string& orth, const Tagset<std::string>& tagset);
    %ignore MorphInterpretation::createWhitespace(int startNode, int endNode, const std::string& orth, const Tagset<std::string>& tagset);
    %ignore Morfeusz::analyze(const char*) const;
    %ignore Morfeusz::analyze(const std::string&) const;
    %ignore Morfeusz::setCharset(Charset);
//    %ignore Morfeusz::analyze(const std::string&, std::vector<MorphInterpretation>&) const;
//    %ignore Morfeusz::generate(const std::string&, std::vector<MorphInterpretation>&) const;
//    %ignore Morfeusz::generate(const std::string&, int, std::vector<MorphInterpretation>&) const;
    %ignore Morfeusz::setDebug(bool);
    
    %newobject Morfeusz::createInstance();
    %newobject Morfeusz::analyzeAsIterator(const char*) const;
}

%extend morfeusz::Morfeusz {
    morfeusz::ResultsIterator* morfeusz::Morfeusz::analyzeAsIterator(const char* text) const {
        return dynamic_cast<const morfeusz::MorfeuszInternal*>($self)->analyzeWithCopy(text);
    }
}

%template(InterpsList) std::vector<morfeusz::MorphInterpretation>;
%template(StringsList) std::vector<std::string>;

%include "../morfeusz2.h"


#ifdef SWIGPYTHON
%include "morfeusz_python.i"
#endif
