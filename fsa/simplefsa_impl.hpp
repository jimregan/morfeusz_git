/* 
 * File:   simplefsa_impl.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 1:05 PM
 */

#ifndef SIMPLEFSA_IMPL_HPP
#define	SIMPLEFSA_IMPL_HPP

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct StateData {
    unsigned transitionsNum : 7;
    unsigned accepting : 1;
};

#pragma pack(pop)   /* restore original alignment from stack */

template <class T>
SimpleFSA<T>::SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr, deserializer) {
}

template <class T>
SimpleFSA<T>::~SimpleFSA() {

}

static unsigned int decodeOffset(const unsigned char* ptr) {
    unsigned int res = 0;
    res = ptr[0] << 16 | ptr[1] << 8 | ptr[2];
    return res;
}

template <class T>
void SimpleFSA<T>::proceedToNext(const char c, State<T>& state) const {
    const unsigned char* fromPointer = this->initialStatePtr + state.getOffset();
    int transitionsTableOffset = sizeof (StateData);
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
    }
    StateData stateData = *(StateData*) (fromPointer);
    const unsigned char* foundTransition = fromPointer + transitionsTableOffset;
    bool found = false;
    for (int i = 0; i < stateData.transitionsNum; i++, foundTransition += 4) {
        if ((char) *foundTransition == c) {
            found = true;
            break;
        }
    }
    //    const_cast<Counter*>(&counter)->increment(foundTransition - transitionsStart + 1);
    if (!found) {
        state.setNextAsSink();
    }
    else {
        unsigned int offset = decodeOffset(foundTransition + 1);
        const unsigned char* nextStatePointer = this->initialStatePtr + offset;
        StateData* nextStateData = (StateData*) (nextStatePointer);
        if (nextStateData->accepting) {
            T object;
            int size = this->deserializer.deserialize(nextStatePointer + sizeof (StateData), object);
            state.setNext(offset, object, size);
        } else {
            state.setNext(offset);
        }
    }
}

#endif	/* SIMPLEFSA_IMPL_HPP */

