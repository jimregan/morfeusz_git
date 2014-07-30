%typemap(javaimports) morfeusz::Morfeusz %{
import java.io.IOException;
import java.util.List;
%}

%typemap(javaimports) std::vector %{
import java.util.List;
import java.util.AbstractList;
%}

%javaexception("IOException") morfeusz::Morfeusz::setAnalyzerDictionary {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%javaexception("IOException") morfeusz::Morfeusz::setGeneratorDictionary {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%typemap(javabase) std::vector<morfeusz::MorphInterpretation> "AbstractList<MorphInterpretation>"
%typemap(javabase) std::vector<morfeusz::String> "AbstractList<String>"

%typemap(javacode) morfeusz::Morfeusz %{
    
    public List<MorphInterpretation> analyze(String text) {
        InterpsList res = new InterpsList();
        analyze(text, res);
        return res;
    }
    
//    public ResultsIterator analyzeAsIterator(String text) {
//        ResultsIterator res = analyze(text);
//        res.setTextReference(text);
//        return res;
//    }
    
    public List<MorphInterpretation> generate(String text) {
        InterpsList res = new InterpsList();
        generate(text, res);
        return res;
    }
    
    public List<MorphInterpretation> generate(String text, int tagnum) {
        InterpsList res = new InterpsList();
        generate(text, tagnum, res);
        return res;
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

%javamethodmodifiers morfeusz::Morfeusz::analyze(const std::string&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::analyze(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, std::vector<MorphInterpretation>&) const "private";
%javamethodmodifiers morfeusz::Morfeusz::generate(const std::string&, int, std::vector<MorphInterpretation>&) const "private";

// ignore size_t std::vector<T>::size() function as it gives "long" return type instead of "int"
%rename("$ignore", regextarget=1, fullname=1) "std::vector<.*>::size$";

%typemap(javaclassmodifiers) std::vector "class"

%typemap(javacode) std::vector %{
    public int size() {
        return getSizeAsInt();
    }
%}

%extend std::vector {
//    T& std::vector::get(int idx) {
//        return at(idx);
//    }
    
//    void std::vector::set(int idx, const T& object) {
//        (*this)[idx] = object;
//    }
    
    void std::vector::add(int idx, const T& object) {
        insert(begin() + idx, object);
    }
    
    T std::vector::remove(int idx) {
        T res = this->at(idx);
        erase(begin() + idx);
        return res;
    }
    
    int getSizeAsInt() const {
        return size();
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
