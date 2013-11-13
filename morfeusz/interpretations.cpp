
#include "interpretations.hpp"
#include "Tagset.hpp"

using namespace std;

string TaggedInterpretation::toString() const {
    std::stringstream ss;
    ss << lemma << ":" << tag << ":" << name;
    return ss.str();
}

template <class T>
string InterpretationsDecoder<T>::convertLemma(
        const string& orth,
        const EncodedLemma& lemma) const {
    string res(orth);
    res.erase(
            res.end() - lemma.suffixToCut,
            res.end());
    res.append(lemma.suffixToAdd);
    return res;
}

RawInterpretation RawInterpretationsDecoder::getInterpretation(
        const string& orth,
        const EncodedInterpretation& interp) const {
    string lemma = this->convertLemma(orth, interp.lemma);
    RawInterpretation res = {lemma, interp.tag, interp.nameClassifier};
    return res;
}

TaggedInterpretationsDecoder::TaggedInterpretationsDecoder(const Tagset& tagset)
: tagset(tagset) {
    
}

TaggedInterpretation TaggedInterpretationsDecoder::getInterpretation(
        const string& orth,
        const EncodedInterpretation& interp) const {
    string lemma = this->convertLemma(orth, interp.lemma);
    const string& tag = this->tagset.getTag(interp.tag);
    const string& name = this->tagset.getName(interp.nameClassifier);
    TaggedInterpretation res = {lemma, tag, name};
    return res;
}
