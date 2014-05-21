
// XXX
// set module name with preprocessor
// because Mac OS X doesn't seem to recognize jniclassname option
#ifdef SWIGJAVA
%module MorfeuszWrapper
#else
%module morfeusz2
#endif 

%feature("autodoc", "2");
%{
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"
#include "exceptions.hpp"
#include "const.hpp"
%}

%include "std_vector.i"
%include "std_string.i"
%include "std_except.i"
%include "exception.i"

%exception
{
    try{
        $action
    }
    catch(const FileFormatException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.what()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

#ifdef SWIGJAVA

%typemap(javaimports) Morfeusz %{
import java.io.IOException;
%}

%javaexception("java.io.IOException") Morfeusz::setAnalyzerFile {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%javaexception("java.io.IOException") Morfeusz::setGeneratorFile {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%include "enums.swg"

/* Force the generated Java code to use the C enum values rather than making a JNI call */
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("jmorfeusz");
  }
%}
#endif

%ignore MorphInterpretation::MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const std::string& lemma,
            int tagnum,
            int namenum,
            const Tagset& tagset,
            const CharsetConverter& charsetConverter);

%ignore MorphInterpretation::createIgn(int startNode, const std::string& orth, const Tagset& tagset, const CharsetConverter& charsetConverter);

%ignore Tagset::Tagset(const unsigned char* fsaData);

%include "Morfeusz.hpp"
%include "MorphInterpretation.hpp"
%include "const.hpp"
%include "exceptions.hpp"

// instantiate vector of interpretations
namespace std {
       // dirty hack so it will compile without no-arg constructor for MorphInterpretation
   %ignore vector<MorphInterpretation>::vector(size_type); 
   %ignore vector<MorphInterpretation>::resize;
   
   %template(InterpsVector) vector<MorphInterpretation>;
}

#ifdef SWIGPYTHON
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
#endif
