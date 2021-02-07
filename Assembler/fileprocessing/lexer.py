from sly import Lexer

MODE_IMMEDIATE = 'IMMEDIATEADDR'
MODE_ZEROPAGE = 'ZEROPAGEADDR'
MODE_ABSOLUTE = 'ABSOLUTEADDR'

class AsmLexer(Lexer):
    
    tokens = { OPCODE, COMMENT, IMMEDIATEADDR, ZEROPAGEADDR, ABSOLUTEADDR }
    ignore = ' \t\n'

    OPCODE          = r'[A-Z]{3}'
    COMMENT         = r';.*$'
    IMMEDIATEADDR   = r'\#\$[0-9a-fA-F]{1,2}'       # ex. #$1  
    ABSOLUTEADDR    = r'\$[0-9a-fA-F]{4}'           # ex. $4fe4
    ZEROPAGEADDR    = r'\$[0-9a-fA-F]{1,2}'         # ex. $7f



