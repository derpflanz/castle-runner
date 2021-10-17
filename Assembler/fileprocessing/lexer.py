from sly import Lexer

MODE_IMMEDIATE = '#'
MODE_ZEROPAGE = 'zp'
MODE_ABSOLUTE = 'a'
MODE_ABSINDEXX = 'a,x'
MODE_ABSINDEXY = 'a,y'
MODE_ZPINDINDY = '(zp),y'
MODE_RELATIVE = 'r'

TOK_OPCODE = 'OPCODE'
TOK_ASCII = 'ASCII'
TOK_COMMENT = 'COMMENT'

# will be unused in the assembler
TOK_LABEL = 'LABEL'
TOK_ABSINDEX = 'ABSINDEX'
TOK_STRINGNAME = 'STRINGNAME'
TOK_STRING = 'STRING'
TOK_INCLUDE = 'INCLUDE'
TOK_HI = 'HI'
TOK_LO = 'LO'

class AsmLexer(Lexer):
    tokens = { OPCODE, COMMENT, IMMEDIATEADDR, ZEROPAGEADDR, ABSINDEXX, ABSINDEXY, ABSOLUTEADDR, ZPINDINDY, ASCII, \
        LABEL, ABSINDEX, STRINGNAME, STRING, INCLUDE, LO, HI }
    ignore = ' \t\n'

    # regexes for tokens: order matters!

    # stuff that needs to be processed
    OPCODE          = r'[A-Z]{3}'

    # operands
    IMMEDIATEADDR   = r'\#\$[0-9a-fA-F]{1,2}|(LO|HI)\(\$[0-9a-fA-F]{4}\)'       # ex. #$1 or LO($8000)  

    # real addressing modes: will go into _to_bytes(address)
    ABSINDEXX       = r'\$[0-9a-fA-F]{4},[Xx]'      # ex. $7f,X
    ABSINDEXY       = r'\$[0-9a-fA-F]{4},[Yy]'      # ex. $7f,Y
    ABSOLUTEADDR    = r'\$[0-9a-fA-F]{4}'           # ex. $4fe4
    ZEROPAGEADDR    = r'\$[0-9a-fA-F]{1,2}'         # ex. $7f
    ZPINDINDY       = r'\(\$[0-9a-fA-F]{2}\),[Yy]'  # ex. ($10),Y

    # extra stuff
    ASCII           = r'\'[a-zA-Z0-9_*\-\\/<>?:\";\'{}|\[\]~`!@#$%^&*()=+]\''
    STRING          = r'".*"'
    COMMENT         = r';.*$'

    # stuff that only needs to be recognised by the preprocessing
    ABSINDEX        = r'@[a-zA-Z]+,[XxYy]'          # ex. @MSG,X
    STRINGNAME      = r'@[a-zA-Z]+'
    LABEL           = r':[a-zA-Z_][_a-zA-Z0-9]*'      # ex. :label1
    INCLUDE         = r'#include'
    LO              = r'LO(.*)'
    HI              = r'HI(.*)'


