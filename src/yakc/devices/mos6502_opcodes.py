#-------------------------------------------------------------------------------
#   mos6502_opcodes.py
#   Generate 6502 instruction decoder.
#-------------------------------------------------------------------------------

# fips code generator version stamp
Version = 2

# tab-width for generated code
TabWidth = 2

# the target file handle
Out = None

# instruction name table for method names
op_fun = [
    # cc=00
    [ None, 'bit', 'jmp', 'jmpi', 'sty', 'ldy', 'cpy', 'cpx' ],
    # cc=01
    [ 'ora', 'anda', 'eor', 'adc', 'sta', 'lda', 'cmp', 'sbc' ],
    # cc=10
    [ 'asl', 'rol', 'lsr', 'ror', 'stx', 'ldx', 'dec', 'inc' ],
    # cc=11
    [ None, None, None, None, None, None, None, None ]
]

# implied instructions
op_implied_fun = {
    0x00:   'brk()',
    0x20:   'jsr()',
    0x40:   'rti()',
    0x60:   'rts()',

    0x08:   'php()',
    0x28:   'plp()',
    0x48:   'pha()',
    0x68:   'pla()',
    0x88:   'dey()',
    0xA8:   'tay()',
    0xC8:   'iny()',
    0xE8:   'inx()',

    0x18:   'cl(CF)',
    0x38:   'se(CF)',
    0x58:   'cl(IF)',
    0x78:   'se(IF)',
    0x98:   'tya()',
    0xB8:   'cl(VF)',
    0xD8:   'cl(DF)',
    0xF8:   'se(DF)',

    0x8A:   'txa()',
    0x9A:   'txs()',
    0xAA:   'tax()',
    0xBA:   'tsx()',
    0xCA:   'dex()',
    0xEA:   'nop()',
}

# instruction type / address mode table
#   '#'     immediate
#   'A'     accumulator
#   'a'     absolute 
#   'z'     zero page
#   'ax'    absolute indexed with X
#   'ay'    absolute indexed with Y
#   'zx'    zero page indexed with X
#   'zy'    zero page indexed with Y
#   'ix'    indexed indirect
#   'iy'    indirect indexed
#   'x'     implied instruction
#   '-'     invalid instruction
#
op_tbl = [
    # cc=00
    [
        # xxx   BIT   JMP   JMP() STY   LDY   CPY   CPX
        [ 'x',  'x',  'x',  'x',  '-',  '#',  '#',  '#' ],   # bbb=000
        [ '-',  'z',  '-',  '-',  'z',  'z',  'z',  'z' ],   # bbb=001
        [ 'x',  'x',  'x',  'x',  'x',  'x',  'x',  'x' ],   # bbb=010
        [ '-',  'a',  'a',  'a',  'a',  'a',  'a',  'a' ],   # bbb=011
        [ 'x',  'x',  'x',  'x',  'x',  'x',  'x',  'x' ],   # bbb=100 
        [ '-',  '-',  '-',  '-',  'zx', 'zx', '-',  '-' ],   # bbb=101
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],   # bbb=110
        [ '-',  '-',  '-',  '-',  '-',  'ax',  '-',  '-'],   # bbb=111
    ],
    # cc=01
    [
        # ORA   AND   EOR   ADC   STA   LDA   CMP   SBC
        [ 'ix', 'ix', 'ix', 'ix', 'ix', 'ix', 'ix', 'ix' ],  # bbb=000
        [ 'z',  'z',  'z',  'z',  'z',  'z',  'z',  'z'  ],  # bbb=001
        [ '#',  '#',  '#',  '#',  '-',  '#',  '#',  '#'  ],  # bbb=010
        [ 'a',  'a',  'a',  'a',  'a',  'a',  'a',  'a'  ],  # bbb=011
        [ 'iy', 'iy', 'iy', 'iy', 'iy', 'iy', 'iy', 'iy' ],  # bbb=100 
        [ 'zx', 'zx', 'zx', 'zx', 'zx', 'zx', 'zx', 'zx' ],  # bbb=101
        [ 'ay', 'ay', 'ay', 'ay', 'ay', 'ay', 'ay', 'ay' ],  # bbb=110
        [ 'ax', 'ax', 'ax', 'ax', 'ax', 'ax', 'ax', 'ax' ],  # bbb 111
    ],
    # cc=10
    [
        # ASL   ROL   LSR   ROR   STX   LDX   DEC   INC
        [ '-',  '-',  '-',  '-',  '-',  '#',  '-',  '-'  ],  # bbb=000
        [ 'z',  'z',  'z',  'z',  'z',  'z',  'z',  'z'  ],  # bbb=001
        [ 'A',  'A',  'A',  'A',  'x',  'x',  'x',  'x'  ],  # bbb=010
        [ 'a',  'a',  'a',  'a',  'a',  'a',  'a',  'a'  ],  # bbb=011
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-'  ],  # bbb=100
        [ 'zx', 'zx', 'zx', 'zx', 'zy', 'zy', 'zx', 'zx' ],  # bbb=101
        [ '-',  '-',  '-',  '-',  'x',  'x',  '-',  '-'  ],  # bbb=110
        [ 'ax', 'ax', 'ax', 'ax', 'ay', 'ay', 'ax', 'ax' ],  # bbb=111
    ],
    # cc=11
    [
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
        [ '-',  '-',  '-',  '-',  '-',  '-',  '-',  '-' ],  
    ] 
]

# map addressing mode to loading code
addr_fun = {
    '#':    'addr_imm()',
    'A':    '',
    'a':    'addr_a()',
    'z':    'addr_z()',
    'ax':   'addr_ai(X)',
    'ay':   'addr_ai(Y)',
    'zx':   'addr_zi(X)',
    'zy':   'addr_zi(Y)',
    'ix':   'addr_ix()',
    'iy':   'addr_iy()',
}

# branch flag bits
br_flags = [ 'NF', 'VF', 'CF', 'ZF' ]

import sys
import genutil

# an 'opcode' wraps the instruction byte, human-readable asm mnemonics,
# and the source code which implements the instruction
class opcode :
    def __init__(self, op) :
        self.byte = op
        self.src = None

#-------------------------------------------------------------------------------
# encode a single opcode by instruction byte
#
def enc_op(op):
    o = opcode(op)

    # split opcode into bit groups
    # http://www.llx.com/~nparker/a2/opcodes.html
    #
    # |aaa|bbb|cc|
    #
    cc = op & 0x03
    bbb = (op >> 2) & 0x07
    aaa = (op >> 5) & 0x07
    am = op_tbl[cc][bbb][aaa]

    if cc==0 and bbb==4:
        # special case: branches
        xx = (op >> 6) & 0x03
        y  = (op >> 5) & 0x01
        m = br_flags[xx]
        v = br_flags[xx] if y == 1 else 0
        o.src = 'br({},{})'.format(m,v)
    elif am != '-':
        if am == 'A':
            # special case accumulator ops (rot and shift)
            o.src = '{}a()'.format(op_fun[cc][aaa])
        elif am == 'x':
            # implied instruction
            o.src = '{}'.format(op_implied_fun[op])
        else:
            # ordinary instruction
            o.src = '{}({})'.format(op_fun[cc][aaa], addr_fun[am])
    return o

#-------------------------------------------------------------------------------
# return a tab-string for given indent level
#
def tab(indent) :
    return ' '*TabWidth*indent

#-------------------------------------------------------------------------------
# output a src line
#
def l(s) :
    Out.write(s+'\n')

#-------------------------------------------------------------------------------
def write_header():
    l('// #version:{}#'.format(Version))
    l('// machine generated, do not edit!')
    l('#include "mos6502.h"')
    l('namespace YAKC {')
    l('uint32_t mos6502::do_op(system_bus* bus) {')
    l('{}cycles=1;'.format(tab(1)))
    l('{}switch(fetch_op()) {{'.format(tab(1)))

#-------------------------------------------------------------------------------
def write_op(op):
    if op.src :
        l('{}case {}: {}; break;'.format(tab(2), hex(op.byte), op.src))
    
#-------------------------------------------------------------------------------
def write_footer():
   l('{}default: invalid_opcode();'.format(tab(2)))
   l('{}}}'.format(tab(1)))
   l('{}return cycles;'.format(tab(1)))
   l('}')
   l('} // namespace YAKC');

#-------------------------------------------------------------------------------
#   main encoder function
#
def do_it(f):
    global Out
    Out = f
    write_header()
    for i in range(0,256):
        write_op(enc_op(i))
    write_footer()

#-------------------------------------------------------------------------------
# fips code generator entry 
#
def generate(input, out_src, out_hdr):
    if genutil.isDirty(Version, [input], [out_src]) :
        with open(out_src, 'w') as f:
            do_it(f)

