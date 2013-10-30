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
        for state in sorted(self.fsa.initialState.dfs(set()), key=state.offset):
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
        self.statesTable = list(reversed(fsa.dfs(set())))
        self.state2Index = dict([(state, idx) for (idx, state) in enumerate(self.statesTable)])
        
        # labels sorted by popularity
        self.sortedLabels = [label for (label, freq) in sorted(self.fsa.label2Freq.iteritems(), key=lambda label, freq: (-freq, label))]
        
        # popular labels table
        self.label2Index = dict([(label, sortedLabels.index(label)) for label in sortedLabels][:31])
    
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
        for label, freq in self.sortedLabels[:31]:
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
        transitions = sorted(state.transitionsMap.iteritems(), key=lambda (label, _): (-next.freq, -self.label2Count[label]))
        thisIdx = self.state2Index[state]
        
        if len(transitions) == 0:
            assert state.isAccepting()
            return bytearray()
        else:
            offsetToStateAfterThis = 0
            stateAfterThis = self.statesTable[thisIdx + 1]
            for reversedN, (label, nextState) in enumerate(reversed(transitions)):
                assert nextState.reverseOffset is not None
                n = len(transitions) - reversedN
                
                popularLabel = self.label2Index[label] < 31
                firstByte = self.label2Index[label] if popularLabel else 31
                
                last = len(transitions) == n
                next = last and stateAfterThis == nextState
                
                if last:
                    firstByte |= VLengthSerializer.LAST_FLAG
                
                offsetSize = 0
                offset = 0
                if not next:
                    offsetSize = 1
                    offset = (stateAfterThis.reverseOffset - nextState.reverseOffset) + offsetSize
                    if offset >= 256:
                        offset += 1
                        offsetSize += 1
                    if offset >= 256 * 256:
                        offset += 1
                        offsetSize += 1
                    assert offset < 256 * 256 * 256 #TODO - przerobic na jakis porzadny wyjatek
                    
                firstByte |= (32 * offsetSize)
                
                res.append(firstByte)
                if not popularLabel:
                    res.append(label)
                # serialize offset in big-endian order
                if offsetSize == 3:
                    res.append((offset & 0xFF0000) >> 16)
                if offsetSize >= 2:
                    res.append((offset & 0x00FF00) >> 8)
                if offsetSize >= 1:
                    res.append(offset & 0x0000FF)
                
        return res
