
#include "interpretations.hpp"
#include "Tagset.hpp"

using namespace std;

Interpretation::Interpretation()
: lemma(), tag(), nameClassifier() {

}

Interpretation::Interpretation(const Lemma& lemma, const int tag, const int name)
: lemma(lemma), tag(tag), nameClassifier(name) {

}

StringInterpretation::StringInterpretation(
        const string& lemma,
        const string& tag,
        const string& name)
: lemma(lemma), tag(tag), name(name) {

}

string StringInterpretation::toString() const {
    std::stringstream ss;
    ss << lemma << ":" << tag << ":" << name;
    return ss.str();
}

string LemmaConverter::convertLemma(
        const string& orth,
        const Lemma& lemma) const {
    string res(orth);
    res.erase(
            res.end() - lemma.suffixToCut,
            res.end());
    res.append(lemma.suffixToAdd);
    return res;
}

InterpretationsConverter::InterpretationsConverter(const unsigned char* data)
: tagset(Tagset(data)) {

}

StringInterpretation InterpretationsConverter::convertInterpretation(
        const string& orth,
        const Interpretation& interp) const {
    string lemma = this->lemmaConverter.convertLemma(orth, interp.lemma);
    const string& tag = this->tagset.getTag(interp.tag);
    const string& name = this->tagset.getName(interp.nameClassifier);
    return StringInterpretation(lemma, tag, name);
}


