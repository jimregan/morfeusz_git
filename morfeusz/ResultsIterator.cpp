
#include <vector>
#include "morfeusz2.h"

namespace morfeusz {
    
    using namespace std;

    ResultsIterator::ResultsIterator(const vector<MorphInterpretation>& res) {
        resultsBuffer.insert(resultsBuffer.begin(), res.begin(), res.end());
    }

    MorphInterpretation ResultsIterator::getNext() {
        MorphInterpretation res = this->resultsBuffer.front();
        this->resultsBuffer.pop_front();
        return res;
    }

    bool ResultsIterator::hasNext() {
        return !resultsBuffer.empty();
    }
}
