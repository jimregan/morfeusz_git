
//%exception {
//    try{
//        $action
//    }
//    catch(const morfeusz::FileFormatException& e) {
//        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
//    }
//    catch(const std::exception& e) {
//        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.what()));
//    }
//    catch(...) {
//        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
//    }
//}

namespace morfeusz {
    
    %ignore Morfeusz::analyse(const char*) const;
    %ignore Morfeusz::analyse(const std::string&) const;
    %ignore Morfeusz::setCharset(Charset);
    %ignore Morfeusz::setDebug(bool);
    %ignore Charset;
    
    %newobject Morfeusz::createInstance();
    %newobject Morfeusz::analyseAsIterator(const char*) const;
}

%extend morfeusz::Morfeusz {
    morfeusz::ResultsIterator* morfeusz::Morfeusz::analyseAsIterator(const char* text) const {
        return dynamic_cast<const morfeusz::MorfeuszImpl*>($self)->analyseWithCopy(text);
    }
}

%template(InterpsList) std::vector<morfeusz::MorphInterpretation>;
%template(StringsList) std::vector<std::string>;
%template(StringsLinkedList) std::list<std::string>;

#ifndef SWIGPERL
%template(StringsSet) std::set<std::string>;
#endif
