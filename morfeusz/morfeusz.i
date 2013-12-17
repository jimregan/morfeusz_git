
%module MorfeuszWrapper
%feature("autodoc", "2");
%{
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"
#include "const.hpp"
%}

%include "std_vector.i"
%include "std_string.i"

#ifdef SWIGJAVA

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

// instantiate vector of interpretations
namespace std {
       // dirty hack so it will compile without no-arg constructor for MorphInterpretation
   %ignore vector<MorphInterpretation>::vector(size_type); 
   %ignore vector<MorphInterpretation>::resize;
   
   %template(InterpsVector) vector<MorphInterpretation>; 
}
