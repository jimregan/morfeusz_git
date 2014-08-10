
%include "morfeusz_javadoc.i"

%include <stdint.i>
%include <std_except.i>
%include <std_common.i>
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
    
    template<class T> class list {
        public:
        typedef size_t size_type;
        typedef T value_type;
        typedef const value_type& const_reference;
        
        %rename(isEmpty) empty;
        bool empty() const;
        void clear();
        
        %extend {
        
        const_reference get(int32_t i) const throw (std::out_of_range) {
            std::list<T>::const_iterator it = $self->begin();
            std::advance(it, i);
            return *it;
        }
        
        value_type set(int32_t i, const value_type& VECTOR_VALUE_IN) throw (std::out_of_range) {
            std::list<T>::iterator it = $self->begin();
            std::advance(it, i);
            std::string old = *it;
            *it = VECTOR_VALUE_IN;
            return old;
        }
        
        void add(int32_t i, const value_type& VECTOR_VALUE_IN) {
            std::list<T>::iterator it = $self->begin();
            std::advance(it, i);
            $self->insert(it, VECTOR_VALUE_IN);
        }
        
        value_type remove(int32_t i, const value_type& VECTOR_VALUE_IN) throw (std::out_of_range) {
            std::list<T>::iterator it = $self->begin();
            std::advance(it, i);
            std::string old = *it;
            $self->erase(it);
            return old;
        }
        
        int32_t size() const {
            return $self->size();
        }
        }
    };
        
    template<class T> class set {
        public:
        typedef size_t size_type;
        typedef T value_type;
        typedef const value_type& const_reference;
        
        %rename(isEmpty) empty;
        bool empty() const;
        
        %extend {
            
        const_reference get(int32_t i) const throw (std::out_of_range) {
            std::set<T>::const_iterator it = $self->begin();
            std::advance(it, i);
            return *it;
        }
        
        int32_t size() const {
            return $self->size();
        }
        }
    };
}

%typemap(javaimports) morfeusz::Morfeusz %{
import java.io.IOException;
import java.lang.RuntimeException;
import java.util.List;
import java.util.ArrayList;

/**
 * Performs morphological analysis (analyse methods) and syntesis (generate methods).
 * 
 * It is NOT thread-safe
 * but it is possible to use separate Morfeusz instance for each concurrent thread.
 */
%}

%typemap(javaimports) morfeusz::ResultsIterator %{
/**
 * Iterates through morphological analysis and synthesis results.
 * 
 */
%}

%rename(_dictionarySearchPaths) morfeusz::Morfeusz::dictionarySearchPaths;
%rename(_getLabels) morfeusz::IdResolver::getLabels;
%ignore morfeusz::FileFormatException;

%javaexception("java.io.IOException") morfeusz::Morfeusz::setDictionary {
    try {
        $action
    }
    catch(morfeusz::FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
    catch(std::ios_base::failure & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, e.what());
        return $null;
    }
}

//%javaexception("java.io.IOException") morfeusz::Morfeusz::setGeneratorDictionary {
//    try {
//        $action
//    }
//    catch(morfeusz::FileFormatException & e) {
//        jclass clazz = jenv->FindClass("java/io/IOException");
//        jenv->ThrowNew(clazz, "Invalid file format");
//        return $null;
//    }
//    catch(std::ios_base::failure & e) {
//        jclass clazz = jenv->FindClass("java/io/IOException");
//        jenv->ThrowNew(clazz, e.what());
//        return $null;
//    }
//}

%javaexception("java.util.NoSuchElementException") morfeusz::ResultsIterator::next {
    try {
        $action
    }
    catch(std::out_of_range & e) {
        jclass clazz = jenv->FindClass("java/util/NoSuchElementException");
        jenv->ThrowNew(clazz, e.what());
        return $null;
    }
}

%exception {
    try{
        $action
    }
    catch(const morfeusz::MorfeuszException& e) {
        jclass clazz = jenv->FindClass("pl/waw/ipipan/morfeusz/MorfeuszException");
        jenv->ThrowNew(clazz, e.what());
        return $null;
    }
    catch(const std::exception& e) {
        jclass clazz = jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(clazz, e.what());
        return $null;
    }
    catch(const std::string& e) {
        jclass clazz = jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(clazz, e.c_str());
        return $null;
    }
    catch(...) {
        jclass clazz = jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(clazz, "Unknown exception");
        return $null;
    }
}

%typemap(javainterfaces) morfeusz::ResultsIterator "java.util.Iterator<MorphInterpretation>"
%typemap(javabase) std::vector<morfeusz::MorphInterpretation> "java.util.AbstractList<MorphInterpretation>"
%typemap(javabase) std::vector<std::string> "java.util.AbstractList<java.lang.String>"
%typemap(javabase) std::list<std::string> "java.util.AbstractList<java.lang.String>"
%typemap(javabase) std::set<std::string> "java.util.AbstractList<java.lang.String>"
%typemap(javabase) morfeusz::MorfeuszException "java.lang.RuntimeException"

%typemap(javacode) morfeusz::Morfeusz %{
    
    /**
     * Analyze given text and return the results as list.
     * 
     * @param text text for morphological analysis.
     * @return list containing the results of morphological analysis
    */
    public List<MorphInterpretation> analyseAsList(String text) {
        InterpsList res = new InterpsList();
        analyse(text, res);
        return new ArrayList<MorphInterpretation>(res);
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
        return new ArrayList<MorphInterpretation>(res);
    }

    /**
     * Perform morphological synthesis on a given lemma.
     * Limit results to interpretations with the specified tag.
     * 
     * @param lemma lemma to be analysed
     * @param tagnum tag number of result interpretations
     * @return list containing results of the morphological synthesis
     */
    public List<MorphInterpretation> generate(String lemma, int tagnum) {
        InterpsList res = new InterpsList();
        generate(lemma, tagnum, res);
        return new ArrayList<MorphInterpretation>(res);
    }
    
    /**
     * Get list of paths for dictionaries searching
     * 
     * @return modifiable list of paths
     */
    public static List<String> getDictionarySearchPaths() {
        return get_dictionarySearchPaths();
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

%typemap(javacode) morfeusz::IdResolver %{
    
    public java.util.Collection<java.lang.String> getLabels(int labelsId) {
        return _getLabels(labelsId);
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

%javamethodmodifiers morfeusz::Morfeusz::analyse(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, int, std::vector<MorphInterpretation>&) const "private";

// should be overwritten by getDictionarySearchPaths() in typemap(javacode)
%javamethodmodifiers morfeusz::Morfeusz::dictionarySearchPaths "private";

// should be overwritten by getLabels() in typemap(javacode)
%javamethodmodifiers morfeusz::IdResolver::getLabels "private";

%typemap(javaclassmodifiers) std::vector "class"
%typemap(javaclassmodifiers) std::list "class"
%typemap(javaclassmodifiers) std::set "class"

%include "enums.swg"

/* Force the generated Java code to use the C enum values rather than making a JNI call */
%javaconst(1);

%pragma(java) jniclassclassmodifiers="class"
        
%pragma(java) moduleclassmodifiers="class"

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("jmorfeusz");
  }
%}

%pragma(java) jniclassimports=%{
import java.io.IOException;
%}

%include "std_vector.i"
%include "std_string.i"
%include "std_except.i"
%include "exception.i"
%include "typemaps.i"

