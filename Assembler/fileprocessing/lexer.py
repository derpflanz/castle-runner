from sly import Lexer

MODE_IMMEDIATE = 'IMMEDIATEADDR'
MODE_ZEROPAGE = 'ZEROPAGEADDR'
MODE_ABSOLUTE = 'ABSOLUTEADDR'

TOK_OPCODE = 'OPCODE'
TOK_LABEL = 'LABEL'
TOK_ASCII = 'ASCII'
TOK_STRINGNAME = 'STRINGNAME'
TOK_STRING = 'STRING'
class AsmLexer(Lexer):
    
    tokens = { OPCODE, COMMENT, IMMEDIATEADDR, ZEROPAGEADDR, ABSOLUTEADDR, LABEL, ASCII, STRINGNAME, STRING }
    ignore = ' \t\n'

    OPCODE          = r'[A-Z]{3}'
    COMMENT         = r';.*$'
    IMMEDIATEADDR   = r'\#\$[0-9a-fA-F]{1,2}'       # ex. #$1  
    ABSOLUTEADDR    = r'\$[0-9a-fA-F]{4}'           # ex. $4fe4
    ZEROPAGEADDR    = r'\$[0-9a-fA-F]{1,2}'         # ex. $7f
    LABEL           = r':[a-zA-Z][a-zA-Z0-9]*'      # ex. :label1
    ASCII           = r'\'[a-zA-Z0-9_*\-\\/<>?:\";\'{}|\[\]~`!@#$%^&*()=+]\''
    STRINGNAME      = r'@[a-zA-Z]+'
    STRING          = r'".*"'



