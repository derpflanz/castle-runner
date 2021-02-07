import unittest
import sly
from fileprocessing import opcodes, lexer

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

    def test_to_bytes_one_byte(self):
        codes = opcodes.Opcodes(None)

        b = codes._to_bytes('#$1')

        self.assertEqual(b'\x01', b)

    def test_to_bytes_two_byte(self):
        codes = opcodes.Opcodes(None)

        b = codes._to_bytes('$7ffa')

        self.assertEqual(b'\xfa\x7f', b)


    def test_lda_direct(self):
        # arrange
        tokens = [
            self._token('LDA', lexer.TOK_OPCODE),
            self._token('#$1', lexer.MODE_IMMEDIATE)
        ]
        codes = opcodes.Opcodes(tokens)

        # act
        codes.process()
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\xA9\x01', assembly)

    def test_sta_zeropage(self):
        tokens = [
            self._token('STA', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'\x85\x00', assembly)

    def test_lda_zeropage(self):
        tokens = [
            self._token('LDA', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'\xa5\x00', assembly)

    def test_sta_absolute(self):
        tokens = [
            self._token('STA', lexer.TOK_OPCODE),
            self._token('$7ffa', lexer.MODE_ABSOLUTE)
        ]
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'\x8d\xfa\x7f', assembly)

    def test_jmp_absolute(self):
        tokens = [
            self._token('JMP', lexer.TOK_OPCODE),
            self._token('$8004', lexer.MODE_ABSOLUTE)
        ]
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'\x4c\x04\x80', assembly)

    def test_inc_zeropage(self):
        tokens = [
            self._token('INC', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'\xe6\x00', assembly)

    def test_empty(self):
        tokens = []
        codes = opcodes.Opcodes(tokens)

        codes.process()
        assembly = codes.as_bytes()

        self.assertEqual(b'', assembly)

if __name__ == '__main__':
    unittest.main()
