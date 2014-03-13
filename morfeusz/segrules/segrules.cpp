
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

static inline SegrulesFSA* deserializeFSA(const unsigned char*& ptr) {
    uint32_t fsaSize = deserializeUint32(ptr);
//    static SegrulesDeserializer deserializer;
    SegrulesFSA* res = new SegrulesFSA(ptr);
    ptr += fsaSize;
    return res;
}

map<SegrulesOptions, SegrulesFSA*> createSegrulesFSAsMap(const unsigned char* analyzerPtr) {
    map<SegrulesOptions, SegrulesFSA*> res;
    const unsigned char* fsasMapPtr = getFSAsMapPtr(analyzerPtr);
    const unsigned char* currPtr = fsasMapPtr;
    unsigned char fsasNum = *currPtr;
    currPtr++;
    for (unsigned char i = 0; i < fsasNum; i++) {
        SegrulesOptions options = deserializeOptions(currPtr);
        SegrulesFSA* fsa = deserializeFSA(currPtr);
        res[options] = fsa;
    }
    return res;
}
