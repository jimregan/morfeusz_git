
%include "morfeusz_javadoc.i"

%include <stdint.i>
%include <std_except.i>

// make vector compatible with java.util.List interface

namespace std {

    template<class T> class vector {
      public:
        typedef size_t size_type;
        typedef T value_type;
        typedef const value_type& const_reference;
//        vector();
//        vector(size_type n);
//        vector(const vector& o);
        size_type capacity() const;
        void reserve(size_type n);
        %rename(isEmpty) empty;
        bool empty() const;
        void clear();
        void push_back(const value_type& x);
        %extend {
            const_reference get(int32_t i) const throw (std::out_of_range) {
                return $self->at(i);
            }
            value_type set(int32_t i, const value_type& VECTOR_VALUE_IN) throw (std::out_of_range) {
                const T old = $self->at(i);
                $self->at(i) = VECTOR_VALUE_IN;
                return old;
            }
            void add(int32_t i, const value_type& VECTOR_VALUE_IN) {
                $self->insert($self->begin() + i, VECTOR_VALUE_IN);
            }
            int32_t size() const {
              return $self->size();
            }
            void removeRange(int32_t from, int32_t to) {
              $self->erase($self->begin()+from, $self->begin()+to);
            }
        }
    };
}

%typemap(javaimports) morfeusz::Morfeusz %{
import java.io.IOException;
import java.lang.RuntimeException;
import java.util.List;
import java.util.Collections;

/**
 * Performs morphological analysis (analyze methods) and syntesis (generate methods).
 * 
 * It is NOT thread-safe
 * but it is possible to use separate Morfeusz instance for each concurrent thread.
 */
%}

%typemap(javaimports) morfeusz::ResultsIterator %{
import java.util.Iterator;

/**
 * Iterates through morphological analysis and synthesis results.
 * 
 */
%}

%typemap(javaimports) std::vector %{
import java.util.List;
import java.util.AbstractList;
%}

%javaexception("IOException") morfeusz::Morfeusz::setAnalyzerDictionary {
    try {
        $action
    }
    catch(morfeusz::FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%javaexception("IOException") morfeusz::Morfeusz::setGeneratorDictionary {
    try {
        $action
    }
    catch(morfeusz::FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%typemap(javainterfaces) morfeusz::ResultsIterator "Iterator<MorphInterpretation>"
%typemap(javabase) std::vector<morfeusz::MorphInterpretation> "AbstractList<MorphInterpretation>"
%typemap(javabase) std::vector<morfeusz::String> "AbstractList<String>"
%typemap(javabase) morfeusz::MorfeuszException "RuntimeException"

%typemap(javacode) morfeusz::Morfeusz %{
    
    /**
     * Analyze given text and return the results as list.
     * 
     * @param text text for morphological analysis.
     * @return list containing the results of morphological analysis
    */
    public List<MorphInterpretation> analyzeAsList(String text) {
        InterpsList res = new InterpsList();
        analyze(text, res);
        return Collections.unmodifiableList(res);
    }

    /**
     * Perform morphological synthesis on a given lemma.
     * 
     * @param lemma lemma to be synthesized
     * @return list containing results of the morphological synthesis
     */
    public List<MorphInterpretation> generate(String lemma) {
        InterpsList res = new InterpsList();
        generate(lemma, res);
        return Collections.unmodifiableList(res);
    }

    /**
     * Perform morphological synthesis on a given lemma.
     * Limit results to interpretations with the specified tag.
     * 
     * @param lemma lemma to be analyzed
     * @param tagnum tag number of result interpretations
     * @return list containing results of the morphological synthesis
     */
    public List<MorphInterpretation> generate(String lemma, int tagnum) {
        InterpsList res = new InterpsList();
        generate(lemma, tagnum, res);
        return Collections.unmodifiableList(res);
    }
%}

%typemap(javacode) morfeusz::ResultsIterator %{
    
    /**
     * Removing of elements from this iterator is not supported.
     */
    public void remove() {
        throw new java.lang.UnsupportedOperationException();
    }
%}

%typemap(javafinalize) SWIGTYPE %{
    protected void finalize() {
        if (swigCMemOwn) {
            $moduleJNI.delete_$javaclassname(getCPtr(this));
        }
    }
%}

%typemap(javadestruct, methodname="delete", methodmodifiers="private") SWIGTYPE "";

%javamethodmodifiers morfeusz::Morfeusz::analyze(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, int, std::vector<MorphInterpretation>&) const "private";

%typemap(javaclassmodifiers) std::vector "class"

%include "enums.swg"

/* Force the generated Java code to use the C enum values rather than making a JNI call */
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("jmorfeusz");
  }
%}

%pragma(java) jniclassimports=%{
import java.io.IOException;
%}
