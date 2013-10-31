'''
Created on Oct 20, 2013

@author: mlenart
'''

import logging

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
     
    def _transitionsData2bytearray(self, state):
        res = bytearray()
        transitions = sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): (self.label2Index.get(label, float('inf')), -nextState.freq, -self.label2Count[label]))
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
    VERSION = 2
    FINAL_FLAG =    0b10000000
    LAST_FLAG =     0b01000000
    
    def __init__(self, fsa):
        super(VLengthSerializer2, self).__init__(fsa)
        self.statesTable = list(reversed(list(fsa.dfs())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
    
    def serializePrologue(self):
        res = bytearray()
        
        # serialize magic number in big-endian order
        res.append((VLengthSerializer.MAGIC_NUMBER & 0xFF000000) >> 24)
        res.append((VLengthSerializer.MAGIC_NUMBER & 0x00FF0000) >> 16)
        res.append((VLengthSerializer.MAGIC_NUMBER & 0x0000FF00) >> 8)
        res.append(VLengthSerializer.MAGIC_NUMBER & 0x000000FF)
        
        # serialize version number
        res.append(VLengthSerializer.VERSION)
        
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
     
    def _transitionsData2bytearray(self, state):
        res = bytearray()
        transitions = sorted(state.transitionsMap.iteritems(), key=lambda (label, nextState): (-nextState.freq, -self.label2Count[label]))
        thisIdx = self.state2Index[state]
        logging.debug('state '+str(state.offset))
        if len(transitions) == 0:
            assert state.isAccepting()
#             flags
            return bytearray(0, )
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
