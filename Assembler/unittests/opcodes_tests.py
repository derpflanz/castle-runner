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

        # act
        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(assembly, b'\xea')

    def test_unknown_opcode_raise(self):
        # arrange
        tokens = [
            self._token('XXX', 'OPCODE')
        ]

        # act & assert
        self.assertRaises(opcodes.OpcodeError, opcodes.Opcodes, tokens)

    def test_to_bytes_one_byte(self):
        codes = opcodes.Opcodes(None)

        b = codes._to_bytes('#$1')

        self.assertEqual(b'\x01', b)

    def test_to_bytes_two_byte(self):
        codes = opcodes.Opcodes(None)

        b = codes._to_bytes('$7ffa')

        self.assertEqual(b'\xfa\x7f', b)

    def test_brk(self):
        # arrange
        tokens = [ self._token('BRK', lexer.TOK_OPCODE) ]

        # act
        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\x00', assembly)

    def test_lda_direct(self):
        # arrange
        tokens = [
            self._token('LDA', lexer.TOK_OPCODE),
            self._token('#$1', lexer.MODE_IMMEDIATE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\xA9\x01', assembly)

    def test_ascii(self):
        tokens = [
            self._token('LDA', lexer.TOK_OPCODE),
            self._token("'A'", lexer.TOK_ASCII)
        ]

        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        self.assertEqual(b'\xA9\x41', assembly)

    def test_sta_zeropage(self):
        # arrange
        tokens = [
            self._token('STA', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\x85\x00', assembly)

    def test_lda_zeropage(self):
        # arrange
        tokens = [
            self._token('LDA', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)
        
        # assert
        assembly = codes.as_bytes()

        self.assertEqual(b'\xa5\x00', assembly)

    def test_sta_absolute(self):
        # arrange
        tokens = [
            self._token('STA', lexer.TOK_OPCODE),
            self._token('$7ffa', lexer.MODE_ABSOLUTE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(b'\x8d\xfa\x7f', assembly)

    def test_invalid_lines(self):
        # arrange
        tokens = [
            self._token('STA', lexer.TOK_OPCODE),
            self._token('$7ffa', lexer.MODE_ABSOLUTE),
            self._token('STA', lexer.TOK_OPCODE)
        ]

        # act & assert
        self.assertRaises(SyntaxError, opcodes.Opcodes, tokens)

    def test_length(self):
        # arrange
        tokens = [
            self._token('JMP', lexer.TOK_OPCODE),
            self._token('$8004', lexer.MODE_ABSOLUTE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)

        # assert
        self.assertEqual(3, codes.length())

    def test_jmp_absolute(self):
        # arrange
        tokens = [
            self._token('JMP', lexer.TOK_OPCODE),
            self._token('$8004', lexer.MODE_ABSOLUTE)
        ]

        # act
        codes = opcodes.Opcodes(tokens)
 
        # assert
        self.assertEqual(b'\x4c\x04\x80', codes.as_bytes())

    def test_jmp_label(self):
        # arrange
        tokens = [
            self._token('JMP', lexer.TOK_OPCODE),
            self._token(':loop1', lexer.TOK_LABEL)
        ]

        # act
        codes = opcodes.Opcodes(tokens, {":loop1": "$8004"})
 
        # assert
        self.assertEqual(b'\x4c\x04\x80', codes.as_bytes())



    def test_inc_zeropage(self):
        tokens = [
            self._token('INC', lexer.TOK_OPCODE),
            self._token('$00', lexer.MODE_ZEROPAGE)
        ]

        codes = opcodes.Opcodes(tokens)

        self.assertEqual(b'\xe6\x00', codes.as_bytes())

    def test_empty(self):
        tokens = []

        codes = opcodes.Opcodes(tokens)

        self.assertEqual(b'', codes.as_bytes())



if __name__ == '__main__':
    unittest.main()
