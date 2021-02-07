import unittest
import sly
from fileprocessing import opcodes

class TestOpcodes(unittest.TestCase):
    def _token(self, _value, _type):
        t = sly.lex.Token()
        t.value = _value
        t.type = _type
        t.lineno = 1
        t.index = 1

        return t

    def test_nop(self):
        # arrange
        tokens = [
            self._token('NOP', 'OPCODE')
        ]
        codes = opcodes.Opcodes(tokens)

        # act
        codes.process()
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(assembly, b'\xea')

    def test_unknown_opcode_raise(self):
        # arrange
        tokens = [
            self._token('XXX', 'OPCODE')
        ]

        # act
        codes = opcodes.Opcodes(tokens)

        # assert
        self.assertRaises(opcodes.OpcodeError, codes.process)

    def test_to_bytes(self):
        codes = opcodes.Opcodes(None)

        b = codes._to_bytes('#$1')

        self.assertEqual(b'\x01', b)

    def test_lda_direct(self):
        # arrange
        tokens = [
            self._token('LDA', 'OPCODE'),
            self._token('#$1', 'IMMEDIATEADDR')
        ]
        codes = opcodes.Opcodes(tokens)

        # act
        codes.process()
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\xA9\x01', assembly)





if __name__ == '__main__':
    unittest.main()
