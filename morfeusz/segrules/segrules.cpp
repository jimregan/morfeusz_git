
#include "SegrulesDeserializer.hpp"
#include "segrules.hpp"
#include "../fsa/fsa.hpp"
#include "../fsa/const.hpp"

using namespace std;

static inline uint32_t deserializeUint32(const unsigned char*& ptr) {
    uint32_t res = *reinterpret_cast<const uint32_t*>(ptr);
    res = htonl(res);
    ptr += 4;
    return res;
}

static inline string deserializeString(const unsigned char*& ptr) {
    string res(reinterpret_cast<const char*>(ptr));
    ptr += res.length() + 1;
    return res;
}

static inline const unsigned char* getFSAsMapPtr(const unsigned char* ptr) {
    const unsigned char* additionalDataPtr = ptr 
        + FSA_DATA_OFFSET 
        + ntohl(*reinterpret_cast<const uint32_t*>(ptr + FSA_DATA_SIZE_OFFSET));
    return additionalDataPtr + deserializeUint32(additionalDataPtr) + 4;
}

static inline SegrulesOptions deserializeOptions(const unsigned char*& ptr) {
    SegrulesOptions res;
    res["aggl"] = deserializeString(ptr);
    res["praet"] = deserializeString(ptr);
    return res;
}

static inline SegrulesFSAType* deserializeFSA(const unsigned char*& ptr) {
    uint32_t fsaSize = deserializeUint32(ptr);
    static SegrulesDeserializer deserializer;
    SegrulesFSAType* res = SegrulesFSAType::getFSA(ptr, deserializer);
    ptr += fsaSize;
    return res;
}

map<SegrulesOptions, SegrulesFSAType*> createSegrulesFSAsMap(const unsigned char* analyzerPtr) {
    map<SegrulesOptions, SegrulesFSAType*> res;
    const unsigned char* fsasMapPtr = getFSAsMapPtr(analyzerPtr);
    const unsigned char* currPtr = fsasMapPtr;
    unsigned char fsasNum = *currPtr;
    currPtr++;
    cerr << "FSAs number " << (int) fsasNum << endl;
    for (unsigned char i = 0; i < fsasNum; i++) {
        cerr << "FSA " << (int) i << endl;
        SegrulesOptions options = deserializeOptions(currPtr);
        cerr << options.at("aggl") << " '" << options.at("praet") << "'" << endl;
        SegrulesFSAType* fsa = deserializeFSA(currPtr);
        res[options] = fsa;
    }
    return res;
}
