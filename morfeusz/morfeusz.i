
%module MorfeuszWrapper
%feature("autodoc", "2");
%{
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"
%}

%include "std_vector.i"
%include "std_string.i"

#ifdef SWIGJAVA
%include "enums.swg"
/* Force the generated Java code to use the C enum values rather than making a JNI call */
%javaconst(1);
#endif

// Very simple C++ example for linked list

%include "Morfeusz.hpp"
%include "MorphInterpretation.hpp"
%include "const.hpp"

namespace std {
       // dirty hack so it will compile without no-arg constructor for MorphInterpretation
   %ignore vector<MorphInterpretation>::vector(size_type); 
   %ignore vector<MorphInterpretation>::resize;
   
   %template(InterpsVector) vector<MorphInterpretation>; 
}
