'''
Created on Oct 20, 2013

@author: mlenart
'''

import logging
from state import State

class Serializer(object):

    def __init__(self, fsa):
        self.fsa = fsa
        self.label2Count = {}
        for state in self.fsa.initialState.dfs():
            for label, _ in state.transitionsMap.iteritems():
                self.label2Count[label] = self.label2Count.get(label, 0) + 1
    
    def serialize2CppFile(self, fname):
        res = []
#         self.fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        res.append('const unsigned char DEFAULT_FSA[] = {')
        for byte in self.fsa2bytearray():
            res.append(hex(byte));
            res.append(',');
#         for idx, state in enumerate(sorted(self.fsa.initialState.dfs(set()), key=lambda state: state.offset)):
#             res.append('// state '+str(idx))
#             partRes = []
#             for byte in self.state2bytearray(state):
#                 partRes.append(hex(byte))
#                 partRes.append(',')
#             res.append(' '.join(partRes))
        res.append('}')
        with open(fname, 'w') as f:
            f.write('\n'.join(res))
    
    def serialize2BinaryFile(self, fname):
#         res = bytearray()
#         self.fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
#         for state in sorted(self.fsa.initialState.dfs(set()), key=lambda state: state.offset):
# #             res.append('// state '+str(idx))
#             res.extend(self.state2bytearray(state))
        with open(fname, 'wb') as f:
            f.write(self.fsa2bytearray())
    
    def getStateSize(self, state):
        raise NotImplementedError('Not implemented')
    
    def fsa2bytearray(self):
        
        res = bytearray()
        res.extend(self.serializePrologue())
        self.fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        logging.debug('SERIALIZE')
        for state in sorted(self.fsa.initialState.dfs(set()), key=lambda s: s.offset):
            res.extend(self.state2bytearray(state))
        return res
    
    def state2bytearray(self, state):
        raise NotImplementedError('Not implemented')
    
    def serializePrologue(self):
        raise NotImplementedError('Not implemented')

class SimpleSerializer(Serializer):
    
    ACCEPTING_FLAG = 128
    
    def getStateSize(self, state):
        return 1 + 4 * len(state.transitionsMap.keys()) + self.getDataSize(state)
    
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def state2bytearray(self, state):
        res = bytearray()
        res.extend(self._stateData2bytearray(state))
        res.extend(self._transitionsData2bytearray(state))
        return res
    
    def _stateData2bytearray(self, state):
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= SimpleSerializer.ACCEPTING_FLAG
        firstByte |= len(state.transitionsMap)
        assert firstByte < 256 and firstByte > 0
        res.append(firstByte)
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def _transitionsData2bytearray(self, state):
        res = bytearray()
#         logging.debug('next')
        for byte, nextState in sorted(state.transitionsMap.iteritems(), key=lambda (label, next): (-next.freq, -self.label2Count[label])):
#             logging.debug(byte)
            res.append(byte)
            offset = nextState.offset
            res.append(offset & 0x0000FF)
            res.append((offset & 0x00FF00) >> 8)
            res.append((offset & 0xFF0000) >> 16)
        return res
    
    def serializePrologue(self):
        return bytearray()

class VLengthSerializer(Serializer):
    
    MAGIC_NUMBER = 0x8fc2bc1b
    VERSION = 1
    LAST_FLAG = 128
    
    def __init__(self, fsa):
        super(VLengthSerializer, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.initialState.dfs(set()))))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
        
        # labels sorted by popularity
        self.sortedLabels = [label for (label, freq) in sorted(self.fsa.label2Freq.iteritems(), key=lambda (label, freq): (-freq, label))]
        remainingChars = [c for c in range(256) if not c in self.sortedLabels]
        while len(self.sortedLabels) < 256:
            self.sortedLabels.append(remainingChars.pop())
        
        # popular labels table
        self.label2Index = dict([(label, self.sortedLabels.index(label)) for label in self.sortedLabels][:31])
    
    def serializePrologue(self):
        res = bytearray()
        
        # serialize magic number in big-endian order
        res.append((VLengthSerializer.MAGIC_NUMBER & 0xFF000000) >> 24)
        res.append((VLengthSerializer.MAGIC_NUMBER & 0x00FF0000) >> 16)
        res.append((VLengthSerializer.MAGIC_NUMBER & 0x0000FF00) >> 8)
        res.append(VLengthSerializer.MAGIC_NUMBER & 0x000000FF)
        
        # serialize version number
        res.append(VLengthSerializer.VERSION)
        
        # serialize popular labels
        logging.debug(self.sortedLabels)
        for label in self.sortedLabels[:31]:
            res.append(label)
        
        return res
     
    def getStateSize(self, state):
        return len(self.state2bytearray(state))
     
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def state2bytearray(self, state):
        res = bytearray()
        res.extend(self._stateData2bytearray(state))
        res.extend(self._transitionsData2bytearray(state))
        return res
     
    def _stateData2bytearray(self, state):
        res = bytearray()
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def getKey(self, state, label):
        res = (-state.label2Freq.get(label, 0), -self.fsa.label2Freq.get(label, 0))
#         logging.info(chr(label))
#         logging.info(res)
        return res

    def _transitionsData2bytearray(self, state):
        res = bytearray()
#         logging.info(self.fsa.label2Freq)
        transitions = list(sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): self.getKey(state, label)))
#         logging.info(str([chr(label) for label, _ in transitions]))
        thisIdx = self.state2Index[state]
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
                n = len(transitions) - reversedN
                
                popularLabel = label in self.label2Index
                firstByte = self.label2Index[label] if popularLabel else 31
                
                last = len(transitions) == n
                next = last and stateAfterThis == nextState
                
                if last:
                    firstByte |= VLengthSerializer.LAST_FLAG
                
                offsetSize = 0
                offset = 0
                if not next:
                    offsetSize = 1
#                     nextState.offset - stateAfterThis.offset
                    offset = (stateAfterThis.reverseOffset - nextState.reverseOffset) + offsetSize + len(res) - 1
                    assert offset > 0
                    if offset >= 256:
#                         offset += 1
                        offsetSize += 1
                    if offset >= 256 * 256:
#                         offset += 1
                        offsetSize += 1
                    assert offset < 256 * 256 * 256 #TODO - przerobic na jakis porzadny wyjatek
                    
                firstByte |= (32 * offsetSize)
                
                transitionBytes.append(firstByte)
                if not popularLabel:
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

class VLengthSerializer2(Serializer):
    
    MAGIC_NUMBER = 0x8fc2bc1b
    VERSION = 3
    ACCEPTING_FLAG =    0b10000000
    NEXT_FLAG =         0b01000000
    
    def __init__(self, fsa):
        super(VLengthSerializer2, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.dfs())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
    
    def serializePrologue(self):
        res = bytearray()
        
        # serialize magic number in big-endian order
        res.append((VLengthSerializer2.MAGIC_NUMBER & 0xFF000000) >> 24)
        res.append((VLengthSerializer2.MAGIC_NUMBER & 0x00FF0000) >> 16)
        res.append((VLengthSerializer2.MAGIC_NUMBER & 0x0000FF00) >> 8)
        res.append(VLengthSerializer2.MAGIC_NUMBER & 0x000000FF)
        
        # serialize version number
        res.append(VLengthSerializer2.VERSION)
        
        return res
     
    def getStateSize(self, state):
        return len(self.state2bytearray(state))
     
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def state2bytearray(self, state):
        res = bytearray()
        res.extend(self._stateData2bytearray(state))
        res.extend(self._transitionsData2bytearray(state))
        return res
     
    def _stateData2bytearray(self, state):
        assert len(state.transitionsMap) < 64
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= VLengthSerializer2.ACCEPTING_FLAG
        transitions = list(sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): self.getKey(state, label)))
        if transitions:
            lastLabel, lastNextState = transitions[-1]
            if self.state2Index[lastNextState] == self.state2Index[state] + 1:
                firstByte |= VLengthSerializer2.NEXT_FLAG
        firstByte |= len(state.transitionsMap)
        assert firstByte < 256 and firstByte > 0
        res.append(firstByte)
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def getKey(self, state, label):
        res = (-state.label2Freq.get(label, 0), -self.fsa.label2Freq.get(label, 0))
#         logging.info(chr(label))
#         logging.info(res)
        return res
     
    def _transitionsData2bytearray(self, state):
        res = bytearray()
        transitions = list(sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): self.getKey(state, label)))
        thisIdx = self.state2Index[state]
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
                
                firstByte = label
                
                n = len(transitions) - reversedN
                
                last = len(transitions) == n
                isNext = last and stateAfterThis == nextState
                if not isNext:
                    offsetSize = 0
    #                 offset = 0
                    offset = (stateAfterThis.reverseOffset - nextState.reverseOffset) + len(res)
                    assert offset > 0
                    if offset >= 64:
                        offsetSize += 1
                    if offset >= 256 * 64:
                        offsetSize += 1
                    if offset >= 256 * 256 * 64:
                        offsetSize += 1
                    assert offset < 256 * 256 * 256 * 64 #TODO - przerobic na jakis porzadny wyjatek
                    
                    secondByte = offsetSize
                    secondByte |= (offset >> (offsetSize * 8)) << 2
                    
                    transitionBytes.append(firstByte)
                    transitionBytes.append(secondByte)
                    # serialize offset in big-endian order
                    if offsetSize == 3:
                        transitionBytes.append((offset & 0x00FF0000) >> 16)
                    if offsetSize >= 2:
                        transitionBytes.append((offset & 0x0000FF00) >> 8)
                    if offsetSize >= 1:
                        transitionBytes.append(offset & 0x000000FF)
                    for b in reversed(transitionBytes):
                        res.insert(0, b)
                    logging.debug('inserted transition at beginning '+chr(label)+' -> '+str(offset))
                else:
                    logging.debug('inserted transition at beginning '+chr(label)+' -> NEXT')
                    res.insert(0, firstByte)
        return res

class VLengthSerializer3(Serializer):
    
    MAGIC_NUMBER = 0x8fc2bc1b
    VERSION = 4
    ACCEPTING_FLAG =    0b10000000
    ARRAY_FLAG =         0b01000000
    
    def __init__(self, fsa, useArrays):
        super(VLengthSerializer3, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.dfs())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
        self._chooseArrayStates()
        self.useArrays = useArrays
    
    def serializePrologue(self):
        res = bytearray()
        
        # serialize magic number in big-endian order
        res.append((VLengthSerializer3.MAGIC_NUMBER & 0xFF000000) >> 24)
        res.append((VLengthSerializer3.MAGIC_NUMBER & 0x00FF0000) >> 16)
        res.append((VLengthSerializer3.MAGIC_NUMBER & 0x0000FF00) >> 8)
        res.append(VLengthSerializer3.MAGIC_NUMBER & 0x000000FF)
        
        # serialize version number
        res.append(VLengthSerializer3.VERSION)
        
        # labels sorted by popularity
        self.sortedLabels = [label for (label, freq) in sorted(self.fsa.label2Freq.iteritems(), key=lambda (label, freq): (-freq, label))]
        remainingChars = [c for c in range(256) if not c in self.sortedLabels]
#         while len(self.sortedLabels) < 256:
#             self.sortedLabels.append(remainingChars.pop())
        
        # popular labels table
        self.label2ShortLabel = dict([(label, self.sortedLabels.index(label) + 1) for label in self.sortedLabels[:63]])
        
        logging.debug(dict([(chr(label), shortLabel) for label, shortLabel in self.label2ShortLabel.items()]))
        for label in range(256):
            res.append(self.label2ShortLabel.get(label, 0))
        
        res.append(ord('^'))
        
        return res
     
    def getStateSize(self, state):
        return len(self.state2bytearray(state))
     
    def getDataSize(self, state):
        assert type(state.encodedData) == bytearray or not state.isAccepting()
        return len(state.encodedData) if state.isAccepting() else 0
    
    def state2bytearray(self, state):
        res = bytearray()
        res.extend(self._stateData2bytearray(state))
        res.extend(self._transitionsData2bytearray(state))
        return res
    
    def stateShouldBeAnArray(self, state):
#         return False
#         return len(state.transitionsMap) >= 13
        return self.useArrays and state.serializeAsArray
     
    def _stateData2bytearray(self, state):
        assert len(state.transitionsMap) < 64
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= VLengthSerializer3.ACCEPTING_FLAG
#         transitions = list(sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): self.getKey(state, label)))
#         if transitions:
#             lastLabel, lastNextState = transitions[-1]
#         if self.state2Index[lastNextState] == self.state2Index[state] + 1:
#             firstByte |= VLengthSerializer3.NEXT_FLAG
        if self.stateShouldBeAnArray(state):
            firstByte |= VLengthSerializer3.ARRAY_FLAG
        firstByte |= len(state.transitionsMap)
        assert firstByte < 256 and firstByte > 0
        res.append(firstByte)
        if state.isAccepting():
            res.extend(state.encodedData)
        return res
    
    def getKey(self, state, label):
        res = (-state.label2Freq.get(label, 0))
#         logging.info(chr(label))
#         logging.info(res)
        return res
    
    def _transitions2ListBytes(self, state, originalState=None):
        res = bytearray()
        transitions = list(sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): self.getKey(state, label)))
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
#                     secondByte = offsetSize
#                     secondByte |= (offset >> (offsetSize * 8)) << 2
                
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
    
    def _transition2ArrayBytes(self, state):
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
     
    def _transitionsData2bytearray(self, state):
        if self.stateShouldBeAnArray(state):
            return self._transition2ArrayBytes(state)
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
