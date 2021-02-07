import unittest
import sly
from fileprocessing import opcodes

class TestOpcodes(unittest.TestCase):
    def test_nop(self):
        # arrange
        tok1 = sly.lex.Token()
        tok1.value = 'NOP'
        tok1.type = 'OPCODE'

        tokens = [
            tok1
        ]
        codes = opcodes.Opcodes(tokens)

        # act
        assembly = codes.as_bytes()

        # assert
        self.assertEqual(assembly, b'\xea')

if __name__ == '__main__':
    unittest.main()
