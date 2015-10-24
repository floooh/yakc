#-------------------------------------------------------------------------------
#   opcodes.py
#   Code generator script for Z80 instructions.
#-------------------------------------------------------------------------------

# TODO:
#   LD A,I
#   LD A,R
#       Check the flags set by these instructions (esp P, required 
#       interrupt-requested state-bit
#

Version = 1

from collections import namedtuple
import genutil

# 8-bit register bit masks
R = namedtuple('R8', 'bits name')
r8 = [
    R(0b000, 'B'), 
    R(0b001, 'C'),
    R(0b010, 'D'),
    R(0b011, 'E'),
    R(0b100, 'H'),
    R(0b101, 'L'),
    R(0b111, 'A')
]

# 16-bit register bit masks with SP
r16sp = [
    R(0b00, 'BC'),
    R(0b01, 'DE'),
    R(0b10, 'HL'),
    R(0b11, 'SP')
]

# 16-bit register bit masks with AF
r16af = [
    R(0b00, 'BC'),
    R(0b01, 'DE'),
    R(0b10, 'HL'),
    R(0b11, 'AF')
]

#-------------------------------------------------------------------------------
def add_op(ops, op, src) :
    '''
    Add an opcode with generated source code lines to a
    global opcodes dictionary (the opcode is the key, and
    the src is the value).
    '''
    if op in ops :
        genutil.fmtError('opcode collision!')
    ops[op] = src
    return ops

#-------------------------------------------------------------------------------
def inc_tstates(src, num) :
    src.append('state.T += {};'.format(num))
    return src

#-------------------------------------------------------------------------------
def NOP(ops) :
    '''
    NOP
    T-states: 4
    ''' 
    op = 0b00000000
    src = ['// NOP']
    src = inc_tstates(src, 4)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_s(ops) :
    '''
    LD r,s
    T-states: 4
    '''
    for r in r8 :
        for s in r8 :
            op = 0b01000000 | r.bits<<3 | s.bits
            src = ['// LD {},{}'.format(r.name, s.name)]
            src.append('state.{} = state.{};'.format(r.name, s.name))
            src = inc_tstates(src, 4)
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_n(ops) :
    '''
    LD r,n
    T-states: 7
    '''
    for r in r8 :
        op = 0b00000110 | r.bits<<3
        src = ['// LD {},n'.format(r.name)]
        src.append('state.{} = mem->r8(state.PC++);'.format(r.name))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_iHL(ops) :
    '''
    LD r,(HL)
    T-states: 7
    '''
    for r in r8 :
        op = 0b01000110 | r.bits<<3
        src = ['// LD {},(HL)'.format(r.name)]
        src.append('state.{} = mem->r8(state.HL);'.format(r.name))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_A_iBC(ops) :
    '''
    LD A,(BC)
    T-states: 7
    '''
    op = 0b00001010
    src = ['// LD A,(BC)']
    src.append('state.A = mem->r8(state.BC);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iBC_A(ops) :
    '''
    LD (BC),A
    T-states: 7
    ''' 
    op = 0b00000010
    src = ['// LD (BC),A']
    src.append('mem->w8(state.BC, state.A);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_A_iDE(ops) :
    ''' 
    LD A,(DE)
    T-states: 7
    '''
    op = 0b00011010
    src = ['// LD A,(DE)']
    src.append('state.A = mem->r8(state.DE);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iDE_A(ops) :
    '''
    LD (DE),A
    T-states: 7
    '''
    op = 0b00010010
    src = ['// LD (DE),A']
    src.append('mem->w8(state.DE, state.A);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_A_inn(ops) :
    '''
    LD A,(nn)
    T-states: 13
    '''
    op = 0b00111010
    src = ['// LD A,(nn)']
    src.append('state.A = mem->r8(mem->r16(state.PC));')
    src.append('state.PC += 2;')
    src = inc_tstates(src, 13)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_inn_A(ops) :
    '''
    LD (nn),A
    T-states: 13
    '''
    op = 0b00110010
    src = ['// LD (nn),A']
    src.append('mem->w8(mem->r16(state.PC), state.A);')
    src.append('state.PC += 2;')
    src = inc_tstates(src, 13)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iHL_r(ops) :
    '''
    LD (HL),r
    T-states: 7
    '''
    for r in r8 :
        op = 0b01110000 | r.bits
        src = ['// LD (HL),{}'.format(r.name)]
        src.append('mem->w8(state.HL, state.{});'.format(r.name))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iHL_n(ops) :
    '''
    LD (HL),n
    T-states: 10
    '''
    op = 0b00110110
    src = ['// LD (HL),n']
    src.append('mem->w8(state.HL, mem->r8(state.PC++));')
    src = inc_tstates(src, 10)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_I_A(ops) :
    '''
    LD I,A
    T-states: 9
    '''
    op = 0b01000111
    src = ['// LD I,A']
    src.append('state.I = state.A;')
    src = inc_tstates(src, 9)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_R_A(ops) :
    '''
    LD R,A
    T-states: 9
    '''
    op = 0b01001111
    src = ['// LD R,A']
    src.append('state.R = state.A;')
    src = inc_tstates(src, 9)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_dd_nn(ops) :
    '''
    LD dd,nn
    T-states: 10
    '''
    for r in r16sp :
        op = 0b00000001 | r.bits<<4
        src = ['// LD {},nn'.format(r.name)]
        src.append('state.{} = mem->r16(state.PC);'.format(r.name))
        src.append('state.PC += 2;')
        src = inc_tstates(src, 10)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_HL_inn(ops) :
    '''
    LD HL,(nn)
    T-states: 16
    '''
    op = 0b00101010
    src = ['// LD HL,(nn)']
    src.append('state.HL = mem->r16(mem->r16(state.PC));')
    src.append('state.PC += 2;')
    src = inc_tstates(src, 16)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_inn_HL(ops) :
    '''
    LD (nn),HL
    T-states: 16
    '''
    op = 0b00100010
    src = ['// LD (nn),HL']
    src.append('mem->w16(mem->r16(state.PC), state.HL);')
    src.append('state.PC += 2;')
    src = inc_tstates(src, 16)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_dd_inn(ops) :
    '''
    LD dd,(nn)
    T-states: 20
    '''
    for r in r16sp :
        op = 0b01001011 | r.bits<<4
        src = ['// LD {},(nn)'.format(r.name)]
        src.append('state.{} = mem->r16(mem->r16(state.PC));'.format(r.name))
        src.append('state.PC += 2;')
        src = inc_tstates(src, 20)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_inn_dd(ops) :
    '''
    LD (nn),dd
    T-states: 20
    '''
    for r in r16sp :
        op = 0b01000011 | r.bits<<4
        src = ['// LD (nn),{}'.format(r.name)]
        src.append('mem->w16(mem->r16(state.PC), state.{});'.format(r.name))
        src.append('state.PC += 2;')
        src = inc_tstates(src, 20)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_SP_HL(ops) :
    '''
    LD SP,HL
    T-states: 6
    '''
    op = 0b11111001
    src = ['// LD SP,HL']
    src.append('state.SP = state.HL;')
    inc_tstates(src, 6)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def PUSH_qq(ops) :
    '''
    PUSH qq
    T-states: 11
    '''
    for r in r16af :
        op = 0b11000101 | r.bits<<4
        src = ['// PUSH {}'.format(r.name)]
        src.append('state.SP -= 2;')
        src.append('mem->w16(state.SP, state.{});'.format(r.name))
        src = inc_tstates(src, 11)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def POP_qq(ops) :
    '''
    POP qq
    T-states: 10
    '''
    for r in r16af :
        op = 0b11000001 | r.bits<<4
        src = ['// POP {}'.format(r.name)]
        src.append('state.{} = mem->r16(state.SP);'.format(r.name))
        src.append('state.SP += 2;')
        src = inc_tstates(src, 10)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def PUSH_IXY(ops, xname) :
    '''
    PUSH [IX|IY]
    T-states: 15
    '''
    op = 0b11100101
    src = ['// PUSH {}'.format(xname)]
    src.append('state.SP -= 2;')
    src.append('mem->w16(state.SP, state.{});'.format(xname))
    src = inc_tstates(src, 15)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def POP_IXY(ops, xname) :
    '''
    POP [IX|IY]
    T-states: 14
    '''
    op = 0b11100001
    src = ['// POP {}'.format(xname)]
    src.append('state.{} = mem->r16(state.SP);'.format(xname))
    src.append('state.SP += 2;')
    src = inc_tstates(src, 14)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_iIXY_d(ops, xname) :
    ''' 
    LD r,([IX|IY]+d)
    T-states: 19
    '''
    for r in r8 :
        op = 0b01000110 | r.bits<<3
        src = ['// LD {},({}+d)'.format(r.name, xname)]
        src.append('d = mem->r8(state.PC++);')
        src.append('state.{} = mem->r8(state.{} + d);'.format(r.name, xname))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iIXY_d_n(ops, xname) :
    '''
    LD ([IX|IY]+d),n
    T-states: 19
    '''
    op = 0b00110110
    src = ['// LD ({}+d),n'.format(xname)]
    src.append('d = mem->r8(state.PC++);')
    src.append('mem->w8(state.{} + d, mem->r8(state.PC++));'.format(xname))
    src = inc_tstates(src, 19)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iIXY_d_r(ops, xname) :
    '''
    LD ([IX|IY]+d),r
    T-states: 19
    '''
    for r in r8 :
        op = 0b01110000 | r.bits
        src = ['// LD ({}+d),{}'.format(xname, r.name)]
        src.append('d = mem->r8(state.PC++);')
        src.append('mem->w8(state.{} + d, state.{});'.format(xname, r.name))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_IXY_nn(ops, xname) :
    '''
    LD [IX|IY],nn
    T-states: 14
    '''
    op = 0b00100001
    src = ['// LD {},nn'.format(xname)]
    src.append('state.{} = mem->r16(state.PC);'.format(xname))
    src.append('state.PC += 2;')
    src = inc_tstates(src, 14)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_IXY_inn(ops, xname) :
    '''
    LD [IX|IY],(nn)
    T-states: 20
    '''
    op = 0b00101010
    src = ['// LD {},(nn)'.format(xname)]
    src.append('state.{} = mem->r16(mem->r16(state.PC));'.format(xname))
    src.append('state.PC += 2;')
    src = inc_tstates(src, 20)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_inn_IXY(ops, xname) :
    '''
    LD (nn),[IX|IY]
    T-states: 20
    '''
    op = 0b00100010
    src = ['// LD (nn),{}'.format(xname)]
    src.append('mem->w16(mem->r16(state.PC), state.{});'.format(xname))
    src.append('state.PC += 2;')
    src = inc_tstates(src, 20)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_SP_IXY(ops, xname) :
    '''
    LD SP,[IX|IY]
    T-states: 10
    '''
    op = 0b11111001
    src = ['// LD SP,{}'.format(xname)]
    src.append('state.SP = state.{};'.format(xname))
    src = inc_tstates(src, 10)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def EX_DE_HL(ops) :
    '''
    EX DE,HL
    T-states: 4
    '''
    op = 0b11101011
    src = ['// EX DE,HL']
    src.append('swap16(state.DE, state.HL);')
    src = inc_tstates(src, 4)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def EX_AF_AFx(ops) :
    '''
    EX AF,AF'
    T-states: 4
    '''
    op = 0b00001000
    src = ["// EX AF,AF'"]
    src.append('swap16(state.AF, state.AF_);')
    src = inc_tstates(src, 4)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def EXX(ops) :
    '''
    EXX
    T-states: 4
    '''
    op = 0b11011000
    src = ['// EXX']
    src.append('swap16(state.BC, state.BC_);')
    src.append('swap16(state.DE, state.DE_);')
    src.append('swap16(state.HL, state.HL_);')
    src = inc_tstates(src, 4)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def EX_iSP_HLIXY(ops, xname) :
    '''
    EX (SP),[HL|IX|IY]
    T-states: 19 or 23
    '''
    op = 0b11100011
    src = ['// EX (SP),{}'.format(xname)]
    src.append('u16tmp = mem->r16(state.SP);')
    src.append('mem->w16(state.SP, state.{});'.format(xname))
    src.append('state.{} = u16tmp;'.format(xname))
    if xname == 'HL':
        src = inc_tstates(src, 19)
    else :
        src = inc_tstates(src, 23)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_opcodes() :
    '''
    Generates the single-byte opcode table.
    '''
    ops = {}
    ops = NOP(ops)
    ops = LD_r_s(ops)
    ops = LD_r_n(ops)
    ops = LD_r_iHL(ops)
    ops = LD_iHL_r(ops)
    ops = LD_iHL_n(ops)
    ops = LD_A_iBC(ops)
    ops = LD_iBC_A(ops)
    ops = LD_A_iDE(ops)
    ops = LD_iDE_A(ops)
    ops = LD_A_inn(ops)
    ops = LD_inn_A(ops)
    ops = LD_dd_nn(ops)
    ops = LD_HL_inn(ops)
    ops = LD_inn_HL(ops)
    ops = LD_SP_HL(ops)
    ops = PUSH_qq(ops)
    ops = POP_qq(ops)
    ops = EX_DE_HL(ops)
    ops = EX_AF_AFx(ops)
    ops = EXX(ops)
    ops = EX_iSP_HLIXY(ops, 'HL')
    ops[0xDD] = []
    ops[0xFD] = []
    ops[0xED] = []

    return ops

#-------------------------------------------------------------------------------
def gen_dd_opcodes() :
    '''
    Generate the DD-lead-byte opcode table.
    '''
    dd_ops = {}
    dd_ops = LD_r_iIXY_d(dd_ops, 'IX')
    dd_ops = LD_iIXY_d_r(dd_ops, 'IX')
    dd_ops = LD_iIXY_d_n(dd_ops, 'IX')
    dd_ops = LD_IXY_nn(dd_ops, 'IX')
    dd_ops = LD_IXY_inn(dd_ops, 'IX')
    dd_ops = LD_inn_IXY(dd_ops, 'IX')
    dd_ops = LD_SP_IXY(dd_ops, 'IX')
    dd_ops = PUSH_IXY(dd_ops, 'IX')
    dd_ops = POP_IXY(dd_ops, 'IX')
    dd_ops = EX_iSP_HLIXY(dd_ops, 'IX')
    return dd_ops

#-------------------------------------------------------------------------------
def gen_fd_opcodes() :
    '''
    Generate the FD-lead-byte opcode table.
    '''
    fd_ops = {}
    fd_ops = LD_r_iIXY_d(fd_ops, 'IY')
    fd_ops = LD_iIXY_d_r(fd_ops, 'IY')
    fd_ops = LD_iIXY_d_n(fd_ops, 'IY')
    fd_ops = LD_IXY_nn(fd_ops, 'IY')
    fd_ops = LD_IXY_inn(fd_ops, 'IY')
    fd_ops = LD_inn_IXY(fd_ops, 'IY')
    fd_ops = LD_SP_IXY(fd_ops, 'IY')
    fd_ops = PUSH_IXY(fd_ops, 'IY')
    fd_ops = POP_IXY(fd_ops, 'IY')
    fd_ops = EX_iSP_HLIXY(fd_ops, 'IY')
    return fd_ops

#-------------------------------------------------------------------------------
def gen_ed_opcodes() :
    '''
    Generate the ED-lead-byte opcode table.
    '''
    ed_ops = {}
    ed_ops = LD_I_A(ed_ops)
    ed_ops = LD_R_A(ed_ops)
    ed_ops = LD_dd_inn(ed_ops)
    ed_ops = LD_inn_dd(ed_ops)
    return ed_ops

#-------------------------------------------------------------------------------
def gen_source(f, ops, dd_ops, fd_ops, ed_ops) :
    '''
    Generate the source code from opcode table.
    '''
    
    # write header
    f.write('#pragma once\n')
    f.write('// #version:{}\n'.format(Version))
    f.write('// machine generated, do not edit!\n')
    f.write('#include "yakc/common.h"\n')
    f.write('namespace yakc {\n')
    f.write('inline void z80::step() {\n')
    f.write('    int d;\n')
    f.write('    uword u16tmp;\n')
    f.write('    switch (mem->r8(state.PC++)) {\n')
    for op in ops :
        f.write('    case {}:\n'.format(hex(op)))
        if op == 0xDD :
            # write DD prefix instructions
            f.write('        switch (mem->r8(state.PC++)) {\n')
            for dd_op in dd_ops :
                f.write('        case {}:\n'.format(hex(dd_op)))
                for line in dd_ops[dd_op] :
                    f.write('            {}\n'.format(line))
                f.write('            break;\n')
            f.write('        default:\n')
            f.write('             YAKC_ASSERT(false);\n')
            f.write('        }\n')
        elif op == 0xED :
            # write ED prefix instructions
            f.write('        switch (mem->r8(state.PC++)) {\n')
            for ed_op in ed_ops :
                f.write('        case {}:\n'.format(hex(ed_op)))
                for line in ed_ops[ed_op] :
                    f.write('            {}\n'.format(line))
                f.write('            break;\n')
            f.write('        default:\n')
            f.write('            YAKC_ASSERT(false);\n')
            f.write('        }\n')
        elif op == 0xFD :
            # write FD prefix instructions
            f.write('        switch (mem->r8(state.PC++)) {\n')
            for fd_op in fd_ops :
                f.write('        case {}:\n'.format(hex(fd_op)))
                for line in fd_ops[fd_op] :
                    f.write('            {}\n'.format(line))
                f.write('            break;\n')
            f.write('        default:\n')
            f.write('            YAKC_ASSERT(false);\n')
            f.write('        }\n')
        else :
            for line in ops[op] :
                f.write('        {}\n'.format(line))
        f.write('        break;\n')
    f.write('    default:\n')
    f.write('       YAKC_ASSERT(false);\n')
    f.write('    }\n')
    f.write('}\n')
    f.write('} // namespace yakc\n')

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr) :
    if genutil.isDirty(Version, [input], [out_hdr]) :
        ops = gen_opcodes()
        dd_ops = gen_dd_opcodes()
        fd_ops = gen_fd_opcodes()
        ed_ops = gen_ed_opcodes()
        with open(out_hdr, 'w') as f:
            gen_source(f, ops, dd_ops, fd_ops, ed_ops)


