'''
Created on Oct 20, 2013

@author: mlenart
'''

import logging
from state import State

class Serializer(object):
    
    MAGIC_NUMBER = 0x8fc2bc1b

    def __init__(self, fsa, headerFilename="data/default_fsa.hpp"):
        self._fsa = fsa
        self.headerFilename = headerFilename
    
    @property
    def fsa(self):
        return self._fsa
    
    def getVersion(self):
        return 9
    
    def serialize2CppFile(self, fname):
        res = []
#         self.fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        res.append('\n')
        res.append('#include "%s"' % self.headerFilename)
        res.append('\n')
        res.append('\n')
        res.append('extern const unsigned char DEFAULT_FSA[] = {')
        res.append('\n')
        for byte in self.fsa2bytearray():
            res.append(hex(byte));
            res.append(',');
        res.append('\n')
        res.append('};')
        res.append('\n')
        with open(fname, 'w') as f:
            f.write(''.join(res))
    
    def serialize2BinaryFile(self, fname):
        with open(fname, 'wb') as f:
            f.write(self.fsa2bytearray())
    
    def getStateSize(self, state):
        raise NotImplementedError('Not implemented')
    
    def fsa2bytearray(self):
        res = bytearray()
        res.extend(self.serializePrologue(self.serializeTagset(self.fsa.tagset)))
        self.fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        logging.debug('SERIALIZE')
        for state in sorted(self.fsa.dfs(), key=lambda s: s.offset):
            res.extend(self.state2bytearray(state))
        return res
    
    def serializeTags(self, tagsMap):
        res = bytearray()
        numOfTags = len(tagsMap)
        res.extend(self.htons(numOfTags))
        for tag, tagnum in sorted(tagsMap.iteritems(), key=lambda (tag, tagnum): tagnum):
            res.extend(self.htons(tagnum))
            res.extend(self.fsa.encodeWord(tag))
            res.append(0)
        return res
    
    def serializeTagset(self, tagset):
        res = bytearray()
        if tagset:
            res.extend(self.serializeTags(tagset.tag2tagnum))
            res.extend(self.serializeTags(tagset.name2namenum))
        return res
    
    def htons(self, n):
        assert n < 65536
        assert n >= 0
        res = bytearray()
        res.append((n & 0x00FF00) >> 8)
        res.append(n & 0x0000FF)
        return res
    
    def htonl(self, n):
        assert n >= 0
        res = bytearray()
        res.append((n & 0xFF000000) >> 24)
        res.append((n & 0x00FF0000) >> 16)
        res.append((n & 0x0000FF00) >> 8)
        res.append(n & 0x000000FF)
        return res
    
    def serializePrologue(self, additionalData=None):
        res = bytearray()
        
        # serialize magic number in big-endian order
        res.append((Serializer.MAGIC_NUMBER & 0xFF000000) >> 24)
        res.append((Serializer.MAGIC_NUMBER & 0x00FF0000) >> 16)
        res.append((Serializer.MAGIC_NUMBER & 0x0000FF00) >> 8)
        res.append(Serializer.MAGIC_NUMBER & 0x000000FF)
        
        # serialize version number
        res.append(self.getVersion())
        
        # serialize implementation code 
        res.append(self.getImplementationCode())
        
        # serialize additional data size in 2-byte big-endian
        additionalDataSize = len(additionalData) if additionalData else 0
        res.extend(self.htonl(additionalDataSize))
        
        # add additional data itself
        if additionalDataSize:
            assert type(additionalData) == bytearray
            res.extend(additionalData)
        
        return res
    
    def state2bytearray(self, state):
        res = bytearray()
        res.extend(self.stateData2bytearray(state))
        res.extend(self.transitionsData2bytearray(state))
        return res
    
    def getSortedTransitions(self, state):
        defaultKey = lambda (label, nextState): (-state.label2Freq.get(label, 0), -self.fsa.label2Freq.get(label, 0))
        return list(sorted(
                           state.transitionsMap.iteritems(), 
                           key=defaultKey))
    
    def stateData2bytearray(self, state):
        raise NotImplementedError('Not implemented')
    
    def transitionsData2bytearray(self, state):
        raise NotImplementedError('Not implemented')
    
    def getImplementationCode(self):
        raise NotImplementedError('Not implemented')

class SimpleSerializer(Serializer):
    
    def __init__(self, fsa):
        super(SimpleSerializer, self).__init__(fsa)
        self.ACCEPTING_FLAG = 128
    
    def getImplementationCode(self):
        return 0
    
    def getStateSize(self, state):
        return 1 + 4 * len(state.transitionsMap.keys()) + self.getDataSize(state)
    
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def stateData2bytearray(self, state):
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= self.ACCEPTING_FLAG
        firstByte |= state.transitionsNum
        assert firstByte < 256 and firstByte > 0
        res.append(firstByte)
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def transitionsData2bytearray(self, state):
        res = bytearray()
#         logging.debug('next')
        for label, nextState in self.getSortedTransitions(state):
            res.append(label)
            offset = nextState.offset
            res.append((offset & 0xFF0000) >> 16)
            res.append((offset & 0x00FF00) >> 8)
            res.append(offset & 0x0000FF)
        return res

class VLengthSerializer1(Serializer):
    
    def __init__(self, fsa, useArrays=False):
        super(VLengthSerializer1, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.dfs())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
        self._chooseArrayStates()
        self.useArrays = useArrays
        
        self.ACCEPTING_FLAG =   0b10000000
        self.ARRAY_FLAG =       0b01000000
    
    def getImplementationCode(self):
        return 1
    
    def serializePrologue(self, additionalData):
        res = super(VLengthSerializer1, self).serializePrologue(additionalData)
        
        # labels sorted by popularity
        sortedLabels = [label for (label, freq) in sorted(self.fsa.label2Freq.iteritems(), key=lambda (label, freq): (-freq, label))]
        
        # popular labels table
        self.label2ShortLabel = dict([(label, sortedLabels.index(label) + 1) for label in sortedLabels[:63]])
        
        logging.debug(dict([(chr(label), shortLabel) for label, shortLabel in self.label2ShortLabel.items()]))
        
        # write remaining short labels (zeros)
        for label in range(256):
            res.append(self.label2ShortLabel.get(label, 0))
        
        # write a magic char before initial state
        res.append(ord('^'))
        
        return res
     
    def getStateSize(self, state):
        return len(self.state2bytearray(state))
     
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def stateShouldBeAnArray(self, state):
        return self.useArrays and state.serializeAsArray
     
    def stateData2bytearray(self, state):
        assert state.transitionsNum < 64
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= self.ACCEPTING_FLAG
        if self.stateShouldBeAnArray(state):
            firstByte |= self.ARRAY_FLAG
        firstByte |= state.transitionsNum
        assert firstByte < 256 and firstByte > 0
        res.append(firstByte)
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def _transitions2ListBytes(self, state, originalState=None):
        res = bytearray()
        transitions = self.getSortedTransitions(state)
        thisIdx = self.state2Index[originalState if originalState is not None else state]
        logging.debug('state '+str(state.offset))
        if len(transitions) == 0:
            assert state.isAccepting()
            return bytearray()
        else:
            stateAfterThis = self.statesTable[thisIdx + 1]
            for reversedN, (label, nextState) in enumerate(reversed(transitions)):
                transitionBytes = bytearray()
                assert nextState.reverseOffset is not None
                assert stateAfterThis.reverseOffset is not None
                logging.debug('next state reverse: '+str(nextState.reverseOffset))
                logging.debug('after state reverse: '+str(stateAfterThis.reverseOffset))
                
#                 firstByte = label
                
                n = len(transitions) - reversedN
                hasShortLabel = label in self.label2ShortLabel
                firstByte = self.label2ShortLabel[label] if hasShortLabel else 0
                firstByte <<= 2
                
                last = len(transitions) == n
                isNext = last and stateAfterThis == nextState
                offsetSize = 0
#                 offset = 0
                offset = (stateAfterThis.reverseOffset - nextState.reverseOffset) + len(res)
                assert offset > 0 or isNext
                if offset > 0:
                    offsetSize += 1
                if offset >= 256:
                    offsetSize += 1
                if offset >= 256 * 256:
                    offsetSize += 1
                assert offset < 256 * 256 * 256 #TODO - przerobic na jakis porzadny wyjatek
                assert offsetSize <= 3
                assert offsetSize > 0 or isNext
                firstByte |= offsetSize
                
                transitionBytes.append(firstByte)
                if not hasShortLabel:
                    transitionBytes.append(label)
                # serialize offset in big-endian order
                if offsetSize == 3:
                    transitionBytes.append((offset & 0xFF0000) >> 16)
                if offsetSize >= 2:
                    transitionBytes.append((offset & 0x00FF00) >> 8)
                if offsetSize >= 1:
                    transitionBytes.append(offset & 0x0000FF)
                for b in reversed(transitionBytes):
                    res.insert(0, b)
                logging.debug('inserted transition at beginning '+chr(label)+' -> '+str(offset))

        return res
    
    def _trimState(self, state):
        newState = State()
        newState.encodedData = state.encodedData
        newState.reverseOffset = state.reverseOffset
        newState.offset = state.offset
        newState.transitionsMap = dict([(label, nextState) for (label, nextState) in state.transitionsMap.iteritems()])
#         newState.transitionsMap = dict([(label, nextState) for (label, nextState) in state.transitionsMap.iteritems() if not label in self.label2ShortLabel or not self.label2ShortLabel[label] in range(1,64)])
        newState.serializeAsArray = False
        return newState
    
    def _transitions2ArrayBytes(self, state):
        res = bytearray()
        array = [0] * 64
        for label, nextState in state.transitionsMap.iteritems():
            if label in self.label2ShortLabel:
                shortLabel = self.label2ShortLabel[label]
                array[shortLabel] = nextState.offset
        logging.debug(array)
        for offset in map(lambda x: x if x else 0, array):
            res.append(0)
            res.append((offset & 0xFF0000) >> 16)
            res.append((offset & 0x00FF00) >> 8)
            res.append(offset & 0x0000FF)
        res.extend(self._stateData2bytearray(self._trimState(state)))
        res.extend(self._transitions2ListBytes(self._trimState(state), originalState=state))
        return res
    
    def transitionsData2bytearray(self, state):
        if self.stateShouldBeAnArray(state):
            return self._transitions2ArrayBytes(state)
        else:
            return self._transitions2ListBytes(state)
    
    def _chooseArrayStates(self):
        for state1 in self.fsa.initialState.transitionsMap.values():
            for state2 in state1.transitionsMap.values():
#                 for state3 in state2.transitionsMap.values():
#                     state3.serializeAsArray = True
                state2.serializeAsArray = True
            state1.serializeAsArray = True
        self.fsa.initialState.serializeAsArray = True

class VLengthSerializer2(Serializer):
    
    def __init__(self, fsa, useArrays=False):
        super(VLengthSerializer2, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.dfs())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
        
        self.HAS_REMAINING_FLAG = 128
        self.ACCEPTING_FLAG =    64
        self.LAST_FLAG = 32
    
    def getImplementationCode(self):
        return 2
     
    def getStateSize(self, state):
        return len(self.state2bytearray(state))
     
#     def getDataSize(self, state):
#         assert type(state.encodedData) == bytearray or not state.isAccepting()
#         return len(state.encodedData) if state.isAccepting() else 0
    
    def stateData2bytearray(self, state):
        res = bytearray()
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def _first5Bits(self, number):
        n = number
        while n >= 32:
            n >>= 7
        return n
    
    def _getOffsetBytes(self, offset):
        res = bytearray()
        remaining = offset
        lastByte = True
        while remaining >= 32:
            nextByte = remaining & 0b01111111
            remaining >>= 7
            if not lastByte:
                nextByte |= self.HAS_REMAINING_FLAG
            else:
                lastByte = False
            res.insert(0, nextByte)
            logging.debug(remaining)
        return res
    
    def _transitions2ListBytes(self, state):
        res = bytearray()
        thisIdx = self.state2Index[state]
        transitions = self.getSortedTransitions(state)
        if len(transitions) == 0:
            assert state.isAccepting()
            res.append(self.LAST_FLAG)
            return res
        else:
            stateAfterThis = self.statesTable[thisIdx + 1]
            for reversedN, (label, nextState) in enumerate(reversed(transitions)):
                transitionBytes = bytearray()
                assert nextState.reverseOffset is not None
                assert stateAfterThis.reverseOffset is not None
                logging.debug('next state reverse: '+str(nextState.reverseOffset))
                logging.debug('after state reverse: '+str(stateAfterThis.reverseOffset))
                
                n = len(transitions) - reversedN
                firstByte = label
                
                last = len(transitions) == n
                secondByte = 0
                if last: secondByte |= self.LAST_FLAG
                if nextState.isAccepting(): secondByte |= self.ACCEPTING_FLAG
                offset = (stateAfterThis.reverseOffset - nextState.reverseOffset) + len(res)
                if offset >= 32:
                    secondByte |= self.HAS_REMAINING_FLAG
                    secondByte |= self._first5Bits(offset)
                else:
                    secondByte |= offset
                transitionBytes.append(firstByte)
                transitionBytes.append(secondByte)
                transitionBytes.extend(self._getOffsetBytes(offset))
                for b in reversed(transitionBytes):
                    res.insert(0, b)
                logging.debug('inserted transition at beginning '+chr(label)+' -> '+str(offset))

        return res
#     
    def transitionsData2bytearray(self, state):
        return self._transitions2ListBytes(state)