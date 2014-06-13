
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
            const Tagset& tagset,
            const CharsetConverter& charsetConverter);

    %ignore MorphInterpretation::createIgn(int startNode, const std::string& orth, const Tagset& tagset, const CharsetConverter& charsetConverter);

    %ignore Tagset::Tagset(const unsigned char* fsaData);

}

//%include "../Morfeusz.hpp"
%include "../morfeusz2.h"
//%include "../const.hpp"
//%include "../exceptions.hpp"

// instantiate vector of interpretations
namespace std {
       // dirty hack so it will compile without no-arg constructor for MorphInterpretation
   %ignore vector<MorphInterpretation>::vector(size_type); 
   %ignore vector<MorphInterpretation>::resize;
   
   %template(InterpsVector) vector<MorphInterpretation>;
}

#ifdef SWIGPYTHON
%include "morfeusz_python.i"
#endif
