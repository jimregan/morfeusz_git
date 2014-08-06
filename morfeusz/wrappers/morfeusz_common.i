
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

    %ignore MorphInterpretation::createIgn(int startNode, int endNode, const std::string& orth, const std::string& lemma);
    %ignore MorphInterpretation::createWhitespace(int startNode, int endNode, const std::string& orth);
    %ignore Morfeusz::analyse(const char*) const;
    %ignore Morfeusz::analyse(const std::string&) const;
    %ignore Morfeusz::setCharset(Charset);
//    %rename(_doGetNext) ResultsIterator::next();
//    %ignore Morfeusz::analyse(const std::string&, std::vector<MorphInterpretation>&) const;
//    %ignore Morfeusz::generate(const std::string&, std::vector<MorphInterpretation>&) const;
//    %ignore Morfeusz::generate(const std::string&, int, std::vector<MorphInterpretation>&) const;
    %ignore Morfeusz::setDebug(bool);
    
    %newobject Morfeusz::createInstance();
    %newobject Morfeusz::analyseAsIterator(const char*) const;
}

%extend morfeusz::Morfeusz {
    morfeusz::ResultsIterator* morfeusz::Morfeusz::analyseAsIterator(const char* text) const {
        return dynamic_cast<const morfeusz::MorfeuszInternal*>($self)->analyseWithCopy(text);
    }
}

%template(InterpsList) std::vector<morfeusz::MorphInterpretation>;
%template(StringsList) std::vector<std::string>;
%template(StringsLinkedList) std::list<std::string>;

#ifndef SWIGPERL
%template(StringsSet) std::set<std::string>;
#endif
