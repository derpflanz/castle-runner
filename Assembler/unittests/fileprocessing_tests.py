import unittest
from fileprocessing import lexer

class TestLexer(unittest.TestCase):
    def test_nop(self):
        # arrange
        l = lexer.AsmLexer()
        line = 'NOP'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        self.assertEqual("OPCODE", token.type)

    def test_comment(self):
        # arrange
        l = lexer.AsmLexer()
        line = '; Some comment'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        self.assertEqual("COMMENT", token.type)

    def test_inline_comment(self):
        # arrange
        l = lexer.AsmLexer()
        line = 'NOP ; Some comment'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        self.assertEqual("OPCODE", token.type)
        token = next(result, None)
        self.assertEqual("COMMENT", token.type)

    def test_immediate_address(self):
        # arrange
        l = lexer.AsmLexer()
        line = 'LDA #$1'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        token = next(result, None)
        self.assertEqual("IMMEDIATEADDR", token.type)

    def test_zeropage_address(self):
        # arrange
        l = lexer.AsmLexer()
        line = 'LDA $1'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        token = next(result, None)
        self.assertEqual("ZEROPAGEADDR", token.type)

    def test_absolute_address(self):
        # arrange
        l = lexer.AsmLexer()
        line = 'LDA $7ffa'

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        token = next(result, None)
        self.assertEqual("ABSOLUTEADDR", token.type)

    def test_ascii_value(self):
        # arrange
        l = lexer.AsmLexer()
        line = "LDA 'A'"

        # act
        result = l.tokenize(line)

        # assert
        token = next(result, None)
        token = next(result, None)
        self.assertEqual("ASCII", token.type) 

if __name__ == '__main__':
    unittest.main()
