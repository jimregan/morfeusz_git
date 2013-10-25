'''
Created on Oct 20, 2013

@author: mlenart
'''

class Serializer(object):

    def __init__(self):
        pass
    
    def serialize2CppFile(self, fsa, fname):
        res = []
        fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        res.append('const unsigned char DEFAULT_FSA[] = {')
        for idx, state in enumerate(sorted(fsa.initialState.dfs(set()), key=lambda state: state.offset)):
            res.append('// state '+str(idx))
            partRes = []
            for byte in self.state2bytearray(state):
                partRes.append(hex(byte))
                partRes.append(',')
            res.append(' '.join(partRes))
        res.append('}')
        with open(fname, 'w') as f:
            f.write('\n'.join(res))
    
    def serialize2BinaryFile(self, fsa, fname):
        res = bytearray()
        fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        for state in sorted(fsa.initialState.dfs(set()), key=lambda state: state.offset):
#             res.append('// state '+str(idx))
            res.extend(self.state2bytearray(state))
        with open(fname, 'wb') as f:
            f.write(res)
    
    def getStateSize(self, state):
        raise NotImplementedError('Not implemented')
    
    def fsa2bytearray(self, fsa):
        res = bytearray()
        fsa.calculateOffsets(sizeCounter=lambda state: self.getStateSize(state))
        for state in sorted(fsa.initialState.dfs(set()), key=state.offset):
            res.extend(self.state2bytearray(state))
        return res
    
    def state2bytearray(self, state):
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
        # must sort that strange way because it must be sorted according to char, not unsigned char
        for byte, nextState in sorted(state.transitionsMap.iteritems(), key=lambda (_, state): -state.freq):
            res.append(byte)
            offset = nextState.offset
            res.append(offset & 0x0000FF)
            res.append((offset & 0x00FF00) >> 8)
            res.append((offset & 0xFF0000) >> 16)
        return res
