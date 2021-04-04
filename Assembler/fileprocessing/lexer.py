from sly import Lexer

MODE_IMMEDIATE = 'IMMEDIATEADDR'
MODE_ZEROPAGE = 'ZEROPAGEADDR'
MODE_ABSOLUTE = 'ABSOLUTEADDR'
MODE_ABSINDEXX = 'ABSINDEXX'

TOK_OPCODE = 'OPCODE'
TOK_ASCII = 'ASCII'

# will be unused in the assembler
TOK_LABEL = 'LABEL'
TOK_ABSINDEXX_S = 'ABSINDEXX_S'
TOK_STRINGNAME = 'STRINGNAME'
TOK_STRING = 'STRING'

class AsmLexer(Lexer):
    
    tokens = { OPCODE, COMMENT, IMMEDIATEADDR, ZEROPAGEADDR, ABSINDEXX, ABSOLUTEADDR, ASCII, \
        LABEL, ABSINDEXX_S, STRINGNAME, STRING  }
    ignore = ' \t\n'

    # regexes for tokens: order matters!

    # stuff that needs to be processed
    OPCODE          = r'[A-Z]{3}'
    COMMENT         = r';.*$'
    IMMEDIATEADDR   = r'\#\$[0-9a-fA-F]{1,2}'       # ex. #$1  
    ABSINDEXX       = r'\$[0-9a-fA-F]{4},X'         # ex. $7f,X
    ABSOLUTEADDR    = r'\$[0-9a-fA-F]{4}'           # ex. $4fe4
    ZEROPAGEADDR    = r'\$[0-9a-fA-F]{1,2}'         # ex. $7f
    ASCII           = r'\'[a-zA-Z0-9_*\-\\/<>?:\";\'{}|\[\]~`!@#$%^&*()=+]\''
    STRING          = r'".*"'

    # stuff that only needs to be recognised by the preprocessing
    ABSINDEXX_S     = r'@[a-zA-Z]+,X'               # ex. @MSG,X
    STRINGNAME      = r'@[a-zA-Z]+'
    LABEL           = r':[a-zA-Z][a-zA-Z0-9]*'      # ex. :label1


