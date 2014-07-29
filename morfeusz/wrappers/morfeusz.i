
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
#include "morfeusz2.h"
//#include "exceptions.hpp"
//#include "const.hpp"
//#include "Morfeusz.hpp"
    
using namespace morfeusz;
%}

%include "std_vector.i"
%include "std_string.i"
%include "std_except.i"
%include "exception.i"
%include "typemaps.i"

%exception {
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
%include "morfeusz_java.i"
#endif


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
    
    %newobject Morfeusz::createInstance();
    %newobject Morfeusz::analyze(const std::string&) const;
}

%template(InterpsList) std::vector<morfeusz::MorphInterpretation>;
%template(StringsList) std::vector<string>;

// instantiate vector of interpretations
namespace std {
       // dirty hack so it will compile without no-arg constructor for MorphInterpretation
   %ignore vector<morfeusz::MorphInterpretation>::vector(size_type); 
   %ignore vector<morfeusz::MorphInterpretation>::resize;
   

   //%template(InterpsList) vector<morfeusz::MorphInterpretation>;
   //%template(StringsList) vector<string>;
   
   
}

%include "../morfeusz2.h"


#ifdef SWIGPYTHON
%include "morfeusz_python.i"
#endif
