from sly import Lexer

TOK_OPER_ABSOLUTE = 'ABSOLUTE'
TOK_OPER_ABSINDIND = "ABSINDIND"
TOK_OPER_ABSX = 'ABSINDEXX'
TOK_OPER_ABSY = 'ABSINDEXY'
TOK_OPER_INDIRECT = 'INDIRECT'
TOK_OPER_IMMEDIATE = 'IMMEDIATE'

TOK_OPER_ZP = 'ZEROPAGE'
TOK_OPER_ZPINDIND = 'ZPINDIND'
TOK_OPER_ZPINDX = 'ZPINDX'
TOK_OPER_ZPINDY = 'ZPINDY'
TOK_OPER_ZPIND = 'ZPIND'
TOK_OPER_ZPINDINDY = 'ZPINDINDY'

TOK_OPCODE = 'OPCODE'
TOK_ASCII = 'ASCII'
TOK_COMMENT = 'COMMENT'

# will be unused in the assembler
TOK_LABEL = 'LABEL'
TOK_STRINGDEREF = 'STRINGDEREF'
TOK_STRINGNAME = 'STRINGNAME'
TOK_STRING = 'STRING'
TOK_INCLUDE = 'INCLUDE'
TOK_HI = 'HI'
TOK_LO = 'LO'

class AsmLexer(Lexer):
    tokens = { 
        OPCODE, 
        ABSOLUTE, ABSINDIND, ABSINDEXX, ABSINDEXY, INDIRECT, IMMEDIATE, 
        ZEROPAGE, ZPINDIND, ZPINDX, ZPINDY, ZPIND, ZPINDINDY,
        ASCII, STRING, COMMENT,
        STRINGDEREF, STRINGNAME, LABEL, INCLUDE, HI, LO
    }
    ignore = ' \t\n'

    # regexes for tokens: order matters!

    # stuff that needs to be processed
    OPCODE          = r'[A-Z]{3}'

    # operands
    ABSOLUTE        = r'\$[0-9a-fA-F]{4}'                                   # ex. $4fe4             a       Absolute (1)
    ABSINDIND       = r'\$\([0-9a-fA-F]{4},[Xx]\)'                          # ex. ($4fe4,x)         (a,x)   Absolute Indexed Indirect (2)
    ABSINDEXX       = r'\$[0-9a-fA-F]{4},[Xx]'                              # ex. $7fe4,X           a,x     Absolute Indexed with X (3)
    ABSINDEXY       = r'\$[0-9a-fA-F]{4},[Yy]'                              # ex. $7fe4,Y           a,y     Absolute Indexed with Y (4)
    INDIRECT        = r'\(\$[0-9a-fA-F]{4}\)'                               # ex. ($1000)           (a)     Absolute Indirect (5)
  # ACCU                                                                    #                       A       Accumulator (6)
    IMMEDIATE       = r'\#\$[0-9a-fA-F]{1,2}|(LO|HI)\(\$[0-9a-fA-F]{4}\)'   # ex. #$1 or LO($8000)  #       Immediate (7)
  # IMPLIED                                                                 #                       i       Implied (8)
  # RELATIVE                                                                #                       r       Program Counter Relative (9)
  # STACK                                                                   #                       s       Stack (10)
    ZEROPAGE        = r'\$[0-9a-fA-F]{1,2}'                                 # ex. $7f               zp      Zero Page (11)
    ZPINDIND        = r'\(\$[0-9a-fA-F]{1,2},[Xx]\)'                        # ex. ($7f,x)           (zp,x)  Zero Page Indexed Indirect (12)
    ZPINDX          = r'\$[0-9a-fA-F]{1,2},[Xx]'                            # ex. $7f,x             zp,x    Zero Page Indexed with X (13)
    ZPINDY          = r'\$[0-9a-fA-F]{1,2},[Yy]'                            # ex. $7f,y             zp,y    Zero Page Indexed with Y (14)
    ZPIND           = r'\(\$[0-9a-fA-F]{1,2}\)'                             # ex. ($7f)             (zp)    Zero Page Indirect (15)
    ZPINDINDY       = r'\(\$[0-9a-fA-F]{2}\),[Yy]'                          # ex. ($10),Y           (zp),y  Zero Page Indirect Indexed with Y (16)


    # extra stuff
    ASCII           = r'\'[a-zA-Z0-9_*\-\\/<>?:\";\'{}|\[\]~`!@#$%^&*()=+]\''
    STRING          = r'".*"'
    COMMENT         = r';.*$'

    # stuff that only needs to be recognised by the preprocessing
    STRINGDEREF     = r'@[a-zA-Z]+,[XxYy]'          # ex. @MSG,X
    STRINGNAME      = r'@[a-zA-Z]+'
    LABEL           = r':[a-zA-Z_][_a-zA-Z0-9]*'      # ex. :label1
    INCLUDE         = r'#include'
    LO              = r'LO(.*)'
    HI              = r'HI(.*)'


